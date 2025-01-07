#include "cam_server.h"
#include "esp_timer.h"
#include <WiFi.h>
#include "Arduino.h"
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "ESP32_CAM";
const char *password = "12345678";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
unsigned long lastCommandTime = 0; // Store the timestamp of the last command
int currentStep = -1;              // Track the current step (-1 indicates the function is idle)
void handleForward();
void handleBackward();
void handleLeft();
void handleRight();
void handleStop();
void rotateLeft();
void rotateRight();
void handleSetSpeed();
void diag_for_left ();
void diag_for_right ();
void diag_back_left ();
void diag_back_right ();
void path1();
void path2();
void path3();

int speedValue = 0;

void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("Cannot mount SPIFFS volume...");
  }
}

void initWebServer()
{
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
}
void notifyClients (uint8_t *data) {
  ws.textAll ((char*) data);
}
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg; 
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    notifyClients (data);
    if (!strcmp((char *)data, "forward"))
    { 
      handleForward();
    }
    if (!strcmp((char *)data, "backward"))
    {
      handleBackward();
    }
    if (!strcmp((char *)data, "left"))
    {
      handleLeft();
    }
    if (!strcmp((char *)data, "right"))
    {
      handleRight();
    }
    if (!strcmp((char *)data, "stop"))
    {
      handleStop();
    }
    if (!strcmp((char *)data, "rotateL"))
    {
      rotateLeft();
    }
    if (!strcmp((char *)data, "rotateR"))
    {
      rotateRight();
    }
    if (!strcmp((char *)data, "diag_for_right"))
    {
      diag_for_right();
    }
    if (!strcmp((char *)data, "diag_for_left"))
    {
      diag_for_left();
    }
    if (!strcmp((char *)data, "diag_back_right"))
    {
      diag_back_right();
    }
    if (!strcmp((char *)data, "diag_back_left"))
    {
      diag_back_left();
    }
    if (!strcmp((char *)data, "path1"))
    {
      path1();
    }
    if (!strcmp((char *)data, "path2"))
    {
      path2();
    }
    if (!strcmp((char *)data, "path3"))
    {
      path3();
    }
    else if (String((char *)data).startsWith("setSpeed:"))
    {
      String speedValueStr = String((char *)data).substring(9); // Get the part after "setSpeed:"
      speedValue = speedValueStr.toInt();
      handleSetSpeed();
    }
  }
}

// WebSocket initialization
void onEvent(AsyncWebSocket *server,       //
             AsyncWebSocketClient *client, //
             AwsEventType type,            // the signature of this function is defined
             void *arg,                    // by the AwsEventHandler interface
             uint8_t *data,                //
             size_t len)
{
  switch (type)
  {
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  // Wi-Fi connection
  WiFi.softAP(ssid, password);
  IPAddress miIP = WiFi.softAPIP();

  Serial.begin(115200);
  Serial.setDebugOutput(false);

  // Start cam server
  setup_cam_server();
  // Start streaming web server
  initSPIFFS();
  initWebSocket();
  initWebServer();
  delay(2000);
}

void loop()
{
  ws.cleanupClients();
}

// Handle commands and send them via UART
void handleForward() {
  Serial.printf("l %d'\n", speedValue);
}

void handleBackward() {
  Serial.printf("l -%d'\n", speedValue);
}

void handleLeft() {
  Serial.printf("m %d -%d -%d %d'\n", speedValue, speedValue, speedValue, speedValue);
}

void handleRight() {
  Serial.printf("m -%d %d %d -%d'\n", speedValue, speedValue, speedValue, speedValue);
}

void handleStop() {
  Serial.println("l 0'");
}

void rotateLeft() {
  Serial.printf("m %d %d -%d -%d'\n", speedValue, speedValue, speedValue, speedValue);
}

void rotateRight() {
  Serial.printf("m -%d -%d %d %d'\n", speedValue, speedValue, speedValue, speedValue);
}
// Handle speed adjustment
void handleSetSpeed() {
    
  Serial.print("Speed updated to: ");
  Serial.println(speedValue);
}  

void diag_for_left (){
  Serial.printf("m %d 0 0 %d'\n", speedValue, speedValue);
}
void diag_for_right (){
   Serial.printf("m 0 %d %d 0'\n", speedValue, speedValue);
}
void diag_back_left (){
   Serial.printf("m 0 -%d -%d 0'\n", speedValue, speedValue);
}
void diag_back_right (){
   Serial.printf("m -%d 0 0 -%d'\n", speedValue, speedValue);
}

void path1() {
  static unsigned long lastCommandTime = 0; // Initialize the timer
  static int currentStep = -1; // Initialize the step counter

  unsigned long currentTime = millis(); // Get the current time

  // Check if the function has just been activated
  if (currentStep == -1) {
    Serial.println("Starting square movement...");
    currentStep = 0; // Start with the first step
    lastCommandTime = currentTime; // Initialize the timer
  }
  
  // Use the currentStep to determine the sequence of commands
  switch (currentStep) {
    case 0: // Step 1: Move forward
      if (currentTime - lastCommandTime >= 0) { // Execute immediately
        Serial.println("l 70'"); // Send forward command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 1; // Move to the next step
      }
      break;

    case 1: // Wait 3.3 seconds for forward motion
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'"); // Send right-turn command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 2; // Move to the next step
      }
      break;

    case 2: // Wait 1 second for the turn to complete
      if (currentTime - lastCommandTime >= 1000) {
        Serial.println("l 70'"); // Send forward command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 3; // Move to the next step
      }
      break;

    case 3: // Wait 3.3 seconds for forward motion
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'"); // Send right-turn command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 4; // Move to the next step
      }
      break;

    case 4: // Wait 1 second for the turn to complete
      if (currentTime - lastCommandTime >= 1000) {
        Serial.println("l 70'"); // Send forward command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 5; // Move to the next step
      }
      break;

    case 5: // Wait 3.3 seconds for forward motion
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'"); // Send right-turn command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 6; // Move to the next step
      }
      break;

    case 6: // Wait 1 second for the turn to complete
      if (currentTime - lastCommandTime >= 1000) {
        Serial.println("l 70'"); // Send forward command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 7; // Move to the next step
      }
      break;

    case 7: // Wait 3.3 seconds for forward motion
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'"); // Send right-turn command
        lastCommandTime = currentTime; // Save the current time
        currentStep = 8; // Move to the next step
      }
      break;

    case 8: // Wait 1 second for the turn to complete
      if (currentTime - lastCommandTime >= 1000) {
        Serial.println("Square movement completed.");
        currentStep = -1; // Reset the step counter when done
      }
      break;

    default:
      currentStep = -1; // Reset the step counter when done
      break;
  }
}

void path2() {
  // TODO: Implement triangle path
}

void path3() {
  // TODO: Implement hospital path
}

void loop() {
  path1(); // Execute the path function
}