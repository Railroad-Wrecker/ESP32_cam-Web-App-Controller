#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "Mecanum";
const char* password = "12345678";

// Web server
WebServer server(80);



void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);

  // Configure the ESP32 as an Access Point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();

  // Define web server routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/rotateL", rotateLeft);
  server.on("/rotateR", rotateRight);
  server.on("/setSpeed", handleSetSpeed); // Route to update the speed

  // Start the server
  server.begin();
  delay(2000);
}

void loop() {
  // Handle incoming HTTP requests
  server.handleClient();
}

// HTML page with buttons, speed adjustment, and keyboard control
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Mecanum Robot Control</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          margin: 0;
          padding: 0;
        }
        .grid-container {
          display: grid;
          grid-template-columns: 1fr 1fr 1fr;
          gap: 20px;
          margin: 20px auto;
          max-width: 300px;
        }
        button {
          width: 100px;
          height: 50px;
          font-size: 18px;
          cursor: pointer;
        }
        h1 {
          color: #333;
        }
        .speed-control {
          margin: 20px;
        }
        input {
          width: 80px;
          height: 30px;
          font-size: 16px;
          text-align: center;
        }
      </style>
    </head>
    <body>
      <h1>Mecanum Robot Control</h1>
      <div class="speed-control">
        <label for="speed">Speed (RPM): </label>
        <input type="number" id="speed" value="50" min="0" max="280">
        <button onclick="setSpeed()">Set Speed</button>
      </div>
      <div class="grid-container">
        <button onclick="sendCommand('/forward')">Forward</button>
        <button onclick="sendCommand('/left')">Left</button>
        <button onclick="sendCommand('/stop')">Stop</button>
        <button onclick="sendCommand('/right')">Right</button>
        <button onclick="sendCommand('/backward')">Backward</button>
        <button onclick="sendCommand('/rotateL')">Rotate Left</button>
        <button onclick="sendCommand('/rotateR')">Rotate Right</button>
      </div>
      <p>Use keyboard: W (Forward), S (Backward), A (Left), D (Right), F (Stop), Z (Rotate Left), X (Rotate Right)</p>
      <script>
        let currentSpeed = 50; // Default speed

        // Function to send commands
        function sendCommand(command) {
          fetch(${command}?speed=${currentSpeed});
        }

        // Function to update speed
        function setSpeed() {
          const speedInput = document.getElementById('speed');
          currentSpeed = parseInt(speedInput.value);
          fetch(/setSpeed?value=${currentSpeed});
          alert(Speed set to ${currentSpeed} RPM);
        }

        // Keyboard control
        document.addEventListener('keydown', function(event) {
          if (event.key === 'w' || event.key === 'W') {
            sendCommand('/forward');
          } else if (event.key === 's' || event.key === 'S') {
            sendCommand('/backward');
          } else if (event.key === 'a' || event.key === 'A') {
            sendCommand('/left');
          } else if (event.key === 'd' || event.key === 'D') {
            sendCommand('/right');
          } else if (event.key === 'f' || event.key === 'F') {
            sendCommand('/stop');
          } else if (event.key === 'z' || event.key === 'Z') {
            sendCommand('/rotateL');
          } else if (event.key === 'x' || event.key === 'X') {
            sendCommand('/rotateR');
          }
        });
      </script>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}



