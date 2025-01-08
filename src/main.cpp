#include "cam_server.h"
#include "esp_timer.h"
#include <WiFi.h>
#include "Arduino.h"
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

HardwareSerial SerialAT(1); // Use UART1
unsigned long lastMessageTime = 0;
const unsigned long messageInterval = 1000;  // Send every 1000 ms
int values[5];

const char *ssid = "ESP32_CAM";
const char *password = "12345678";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

unsigned long lastCommandTime = 0; // Store the timestamp of the last command
int currentStep1 = -1;              // Track the current step (-1 indicates the function is idle)
bool path1Active = false;          // Flag to track if path1 is running
int speedValue = 0;
bool path2Active = false;  
int currentStep2 = -1; 
bool path3Active = false;  
int currentStep3 = -1; 


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

void serial_setup() {
  Serial.begin(115200);  // USB Serial for debugging (via FTDI adapter)
  SerialAT.begin(115200, SERIAL_8N1, 15, 14);  // RX=GPIO15, TX=GPIO14
}
void read_serial(){
  if (SerialAT.available()) {
        String receivedData = SerialAT.readStringUntil('\n');  // Read the incoming data
        //Serial.println("Received: " + receivedData);
        int index = 0;
        int start = 0;
        for (int i = 0; i < receivedData.length(); i++) {
            if (receivedData[i] == ' ' || i == receivedData.length() - 1) {
                String value = receivedData.substring(start, i + 1);
                values[index++] = value.toInt();
                start = i + 1;
            }
        }
    }
}

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
    if (!strcmp((char *)data, "path1")) {
      if (!path1Active) {
        path1Active = true; // Activate path1
        currentStep1 = -1;   // Reset path1 to the initial step
      }
    }
    if (!strcmp((char *)data, "path2"))
    {
      if (!path2Active) {
        path2Active = true; // Activate path1
        currentStep2 = -1;   // Reset path1 to the initial step
      }
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
  serial_setup();
  // Wi-Fi connection
  // WiFi.softAP(ssid, password);
  // IPAddress miIP = WiFi.softAPIP();
  WiFi.begin("K", "12345678");
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

void loop() {
  if (path1Active) {
    path1(); // Execute path1 if it's active
  }
  if(path2Active){
    path2();
  }
  if (millis() - lastMessageTime > messageInterval) {
    read_serial();
    StaticJsonDocument<200> jsonDoc;
    JsonArray jsonArray = jsonDoc.to<JsonArray>();

    for (int i = 0; i < 5; i++) {
      jsonArray.add(values[i]); // Generate random numbers between -280 and 281
    }
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    ws.textAll(jsonString); // Broadcast the JSON string to all clients
    lastMessageTime = millis();
  }
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
  if (!path1Active) return;

  unsigned long currentTime = millis();

  if (currentStep1 == -1) {
    currentStep1 = 0;
    lastCommandTime = currentTime;
  }

  switch (currentStep1) {
    case 0:
      if (currentTime - lastCommandTime >= 0) {
        Serial.println("l 70'");
        lastCommandTime = currentTime;
        currentStep1 = 1;
      }
      break;

    case 1:
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'");
        lastCommandTime = currentTime;
        currentStep1 = 2;
      }
      break;

    case 2:
      if (currentTime - lastCommandTime >= 4400) {
        Serial.println("l -70'");
        lastCommandTime = currentTime;
        currentStep1 = 3;
      }
      break;

    case 3:
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m 70 -70 -70 70'");
        lastCommandTime = currentTime;
        currentStep1 = 4;
      }
      break;
    case 4:
      if (currentTime - lastCommandTime >= 4400) {
        Serial.println("l 0'");
        currentStep1 = -1;
        path1Active = false;
      }
      break;

    default:
      currentStep1 = -1;
      path1Active = false;
      break;
  }
}

void path2() {
  if (!path2Active) return;

  unsigned long currentTime = millis();

  if (currentStep2 == -1) {
    currentStep2 = 0;
    lastCommandTime = currentTime;
  }

  switch (currentStep2) {
    case 0:
      if (currentTime - lastCommandTime >= 0) {
        //Serial.println("m 162 162 219 219'");
        Serial.println("m -68.75 -68.75 -122.23 -122.23'");
        lastCommandTime = currentTime;
        currentStep2 = 1;
      }
      break;

    case 1:
      if (currentTime - lastCommandTime >= 12600) {
        Serial.println("l 0'");
        currentStep2 = -1;
        path2Active = false;
      }
      break;
    default:
      currentStep2 = -1;
      path2Active = false;
      break;
  }
}

void path3() {
  if (!path3Active) return;

  unsigned long currentTime = millis();

  if (currentStep3 == -1) {
    currentStep3 = 0;
    lastCommandTime = currentTime;
  }

  switch (currentStep3) {
    case 0:
      if (currentTime - lastCommandTime >= 0) {
        //Serial.println("m 162 162 219 219'");
        Serial.println("m -68.75 -68.75 -122.23 -122.23'");
        lastCommandTime = currentTime;
        currentStep3 = 1;
      }
      break;
    case 1:
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'");
        lastCommandTime = currentTime;
        currentStep3 = 2;
      }
      break;
    case 2:
      if (currentTime - lastCommandTime >= 3300) {
        Serial.println("m -70 70 70 -70'");
        lastCommandTime = currentTime;
        currentStep3 = 3;
      }
      break;
    case 3:
      if (currentTime - lastCommandTime >= 12600) {
        Serial.println("m 70 -70 -70 70'");
        currentStep3 = -1;
        path3Active = false;
      }
      break;
    
    default:
      currentStep3 = -1;
      path3Active = false;
      break;
  }
}