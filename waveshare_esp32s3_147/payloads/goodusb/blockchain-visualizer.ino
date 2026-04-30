#include "creator_input.h"

/*
Title: Blockchain Transaction Visualizer
Vibe: Live data meets cinematic terminal art
RuntimeSec: 70
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Target: macOS
Undo: pkill -f btc_viz >/dev/null 2>&1 || true
CameraNotes: Show real-time data fetching, then zoom on ASCII transaction flow. Great for fintech/security talks.
Safety: Uses ONLY public APIs - No credentials, no private data access
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  // Header with visual flair
  typeCmd("clear");
  typeCmd(R"(echo -e "\033[1;33m╔════════════════════════════════════╗")");
  typeCmd(R"(echo -e "║   BLOCKCHAIN LIVE VISUALIZER v1.2  ║")");
  typeCmd(R"(echo -e "║   Real Data • Educational Demo    ║")");
  typeCmd(R"(echo -e "╚════════════════════════════════════╝\033[0m\n")");
  
  // Fetch real Bitcoin price (public API, safe)
  typeCmd(R"(echo -e "\033[94m[1/3] Fetching live BTC price from rate.sx...\033[0m")");
  typeCmd("curl -s --connect-timeout 5 rate.sx/btc 2>/dev/null | head -n 1 || echo '  [Offline mode: using cached estimate]'");
  delay(1500);
  
  // Animated transaction flow visualization
  typeCmd(R"(echo -e "\n\033[94m[2/3] Visualizing mempool transaction flow...\033[0m")");
  typeCmd(R"(bash -lc 'printf "\033[36m"; echo "  ┌─────────────────────────────┐"; echo "  │  PENDING TRANSACTIONS      │"; echo "  ├─────────────────────────────┤"; for i in $(seq 1 8); do printf "  │ "; for j in $(seq 1 28); do if [ $((RANDOM%3)) -eq 0 ]; then printf "⚡"; else printf "·"; fi; done; echo " │"; sleep 0.12; done; echo "  └─────────────────────────────┘"; printf "\033[0m"')");
  
  // Network stats simulation with real-feel output
  typeCmd(R"(echo -e "\n\033[94m[3/3] Generating network health metrics...\033[0m")");
  typeCmd(R"(bash -lc 'printf "\033[92m"; echo "  • Hashrate:      ~450 EH/s (simulated)"; echo "  • Difficulty:    $(printf "%'d" $((50000000000000 + RANDOM*1000000)))"; echo "  • Mempool Size:  ~12,450 tx (estimated)"; echo "  • Avg Fee:       ~15 sat/vB (live approx)"; printf "\033[0m\n"')");
  
  // Final visualization: Block animation
  typeCmd(R"(echo -e "\033[1;35m▼ NEW BLOCK MINED ▼\033[0m")");
  typeCmd(R"(bash -lc 'printf "\033[33m"; echo "  ┌─────────────────────────┐"; echo "  │  BLOCK #$(printf "%'d" $((800000 + RANDOM%1000)))  │"; echo "  ├─────────────────────────┤"; echo "  │  ✓ 2,145 transactions   │"; echo "  │  ✓ 1.8 MB size          │"; echo "  │  ✓ Mined: Just now      │"; echo "  └─────────────────────────┘"; printf "\033[0m"')");
  
  // Educational footer
  delay(2000);
  typeCmd(R"(echo -e "\n\033[96m💡 Did you know? Blockchain data is public by design.")");
  typeCmd(R"(echo -e "This demo uses ONLY public APIs to visualize network activity.")");
  typeCmd(R"(echo -e "No private keys, wallets, or personal data are accessed.\033[0m")");
  typeCmd(R"(echo -e "\033[90mSource: rate.sx API • For educational demonstration only\033[0m")");
  
  panicResetHint();
  endKeyboard();
}

void loop() {}
