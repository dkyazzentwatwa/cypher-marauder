#include "creator_input.h"

/*
Title: Quantum Decryption Simulator
Vibe: Sci-fi quantum computing terminal
RuntimeSec: 85
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Target: macOS
Undo: pkill -f quantum_sim >/dev/null 2>&1 || true
CameraNotes: Dark room, terminal glow on face. Zoom in on "qubit collapse" animation. End with dramatic reveal.
Safety: PURE VISUAL SIMULATION - No cryptographic operations performed
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  // Opening Title Sequence
  typeCmd("clear");
  typeCmd(R"(echo -e "\033[1;35m╔════════════════════════════════════╗")");
  typeCmd(R"(echo -e "║   QUANTUM DECRYPTOR v3.0 - SIM    ║")");
  typeCmd(R"(echo -e "║   [EDUCATIONAL DEMO ONLY]         ║")");
  typeCmd(R"(echo -e "╚════════════════════════════════════╝\033[0m\n")");
  
  // Phase 1: Quantum State Initialization
  typeCmd(R"(echo -e "\033[94m[1/4] Initializing quantum register...")");
  typeCmd(R"(bash -lc 'printf "\033[36m"; for i in $(seq 1 12); do printf "|%d⟩ " "$((RANDOM%2))"; sleep 0.15; done; echo -e "\n\033[0m"')");
  
  // Phase 2: Superposition Animation
  typeCmd(R"(echo -e "\033[94m[2/4] Entangling qubits via Hadamard gates...")");
  typeCmd(R"(bash -lc 'printf "\033[95m"; for step in 1 2 3; do printf "  Gate H⊗%d: " "$step"; for q in $(seq 1 4); do printf "[α|%d⟩+β|%d⟩] " "$((RANDOM%2))" "$((1-RANDOM%2))"; sleep 0.2; done; echo ""; done; printf "\033[0m"')");
  
  // Phase 3: Fake "Decryption" Progress
  typeCmd(R"(echo -e "\n\033[94m[3/4] Running Shor's algorithm simulation...")");
  typeCmd(R"(bash -lc 'total=30; for i in $(seq 1 $total); do pct=$((i*100/total)); printf "\033[92m["; for j in $(seq 1 $pct); do printf "█" | tr -d "\n"; done; for j in $(seq $((100-pct))); do printf "░" | tr -d "\n"; done; printf "] %d%%\r" "$pct"; sleep 0.08; done; echo -e "\n\033[0m"')");
  
  // Phase 4: Dramatic Result Reveal
  typeCmd(R"(echo -e "\033[94m[4/4] Collapsing wavefunction...")");
  delay(1000);
  typeCmd(R"(echo -e "\n\033[1;93m✨ SIMULATED OUTPUT ✨")");
  typeCmd(R"(echo -e "  Plaintext: [REDACTED_FOR_DEMO]")");
  typeCmd(R"(echo -e "  Key Strength: 2048-bit RSA (SIMULATED BREAK)")");
  typeCmd(R"(echo -e "  Time Elapsed: 0.003s (Quantum Advantage)\033[0m")");
  
  // CRITICAL: Ethical Disclaimer
  delay(2500);
  typeCmd(R"(echo -e "\n\033[1;31m╔════════════════════════════════════╗")");
  typeCmd(R"(echo -e "║  🔐 THIS IS A VISUAL SIMULATION  🔐 ║")");
  typeCmd(R"(echo -e "║  No real encryption was broken.    ║")");
  typeCmd(R"(echo -e "║  For educational purposes only.    ║")");
  typeCmd(R"(echo -e "╚════════════════════════════════════╝\033[0m")");
  typeCmd(R"(echo -e "\n\033[96mQuantum computing poses future risks to current cryptography.")");
  typeCmd(R"(echo -e "Organizations should begin planning for post-quantum cryptography.\033[0m")");
  
  panicResetHint();
  endKeyboard();
}

void loop() {}
