#include "creator_input.h"

/*
Title: AI-Powered Recon Dashboard
Vibe: Futuristic AI analysis terminal
RuntimeSec: 90
BoardSupport: Arduino Keyboard + ESP32-S3 TinyUSB
Target: macOS
Undo: pkill -f ai_recon >/dev/null 2>&1 || true
CameraNotes: Start with tight shot on terminal, pan out as "AI analysis" completes. End with reveal text.
Safety: SIMULATION ONLY - No real network access or data collection
*/

void setup() {
  beginKeyboard();
  waitPreset(WAIT_REEL);
  openTerminal();

  // Scene 1: Initialize AI Interface
  typeCmd("clear");
  typeCmd(R"(echo -e "\033[1;36m╔════════════════════════════════════╗")");
  typeCmd(R"(echo -e "\033[1;36m║  NEURAL RECON v2.1 - AI ANALYSIS  ║")");
  typeCmd(R"(echo -e "\033[1;36m╚════════════════════════════════════╝\033[0m")");
  typeCmd(R"(echo -e "\n\033[94m[INIT] Loading quantum-weighted neural model...")");
  delay(800);
  
  // Scene 2: Fake AI Processing with Visual Feedback
  typeCmd(R"(bash -lc 'for i in $(seq 1 5); do printf "\033[92m[PHASE %d/5] " "$i"; case $i in 1) echo "Feature extraction from network topology";; 2) echo "Anomaly detection via transformer layers";; 3) echo "Cross-referencing threat intelligence feeds";; 4) echo "Generating attack surface heatmap";; 5) echo "Finalizing risk assessment matrix";; esac; sleep 0.6; done')");
  
  // Scene 3: Animated "Results" with ASCII Visualization
  typeCmd(R"(echo -e "\n\033[1;33m▼ ATTACK SURFACE HEATMAP ▼\033[0m")");
  typeCmd(R"(bash -lc 'printf "\033[38;5;196m"; for row in 1 2 3 4 5; do for col in $(seq 1 20); do printf "█"; sleep 0.03; done; echo ""; done; printf "\033[0m"')");
  
  // Scene 4: Fake "Findings" Output
  typeCmd(R"(echo -e "\n\033[1;92m✓ AI ANALYSIS COMPLETE\033[0m")");
  typeCmd(R"(echo -e "\033[94m• Detected 3 potential entry vectors (SIMULATED)")");
  typeCmd(R"(echo -e "• Risk Score: 7.2/10 (Educational Estimate)")");
  typeCmd(R"(echo -e "• Recommended Actions: Patch, Monitor, Train\033[0m")");
  
  // Scene 5: The Reveal - Critical for Ethical Demo
  delay(2000);
  typeCmd(R"(echo -e "\n\033[1;31m╔════════════════════════════════════╗")");
  typeCmd(R"(echo -e "║  ⚠️  TRAINING SIMULATION ONLY  ⚠️  ║")");
  typeCmd(R"(echo -e "║  NO REAL SYSTEMS WERE ACCESSED  ║")");
  typeCmd(R"(echo -e "╚════════════════════════════════════╝\033[0m")");
  typeCmd(R"(echo -e "\n\033[96mThis demo illustrates how AI could enhance security reconnaissance.")");
  typeCmd(R"(echo -e "Always obtain explicit written permission before testing any system.\033[0m")");
  
  panicResetHint();
  endKeyboard();
}

void loop() {}
