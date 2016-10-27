// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <aREST.h>

// PINS
#define PIN_BOARD_LED 0
#define PIN_VIBRATOR 16

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "vibrator";
const char* password = "";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);
IPAddress ip(10, 1, 1, 2);
IPAddress gateway(10,1,1,1);
IPAddress subnet(255,255,255,252);

// Declare functions to be exposed to the API
int rhythmControl(String command);
int speedControl(String command);
int vibrationControl(String command);

// Variables
int rhythmState = 0;
int onBoardLedState = 0;
float vibrateSpeed = 1;
int vibrating = 0;

void setup(void)
{
    // Start Serial
    Serial.begin(115200);

    // Init variables and expose them to REST API
    //rest.variable("on_board_led", &onBoardLedState);

    // Function to be exposed
    rest.function("rhythm", rhythmControl);
    rest.function("speed", speedControl);
    rest.function("vibrating", vibrationControl);

    // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("1");
    rest.set_name("iot_prototype");

    // Connect to WiFi
    WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());

    // Init pins
    pinMode(PIN_BOARD_LED, OUTPUT);
    pinMode(PIN_VIBRATOR, OUTPUT);
}

void loop() {
    handleREST();

    if(vibrating > 0) {
        rhythm();
    } else if(onBoardLedState > 0) {
        flipOnBoardLedState();
    }
}

void handleREST() {
    WiFiClient client = server.available();
    if (client && client.available()) {
        rest.handle(client);
    } else {
        delay(1);
    }
}

void rhythm() {
    if(rhythmState < 0) {
        rhythmState = 0;
    }

    float delayQ = (2 - vibrateSpeed);

    switch(rhythmState) {
        case 0:
            if(onBoardLedState <= 0) {
                flipOnBoardLedState();
            }
            break;
        case 1:
            flipOnBoardLedState();
            delay(500 * delayQ);
            flipOnBoardLedState();
            delay(100 * delayQ);
            flipOnBoardLedState();
            delay(300 * delayQ);
            break;
        case 2:
            flipOnBoardLedState();
            delay(1500 * delayQ);
            break;
    }
}

int rhythmControl(String command) {
    int state = command.toInt();
    rhythmState = state;
}

int speedControl(String command) {
    float newSpeed = command.toFloat();

    if(newSpeed > 195) {
        newSpeed = 195;
    } else if(newSpeed < 1) {
        newSpeed = 1;
    }

    newSpeed = newSpeed / 100;
    vibrateSpeed = newSpeed;
}

int vibrationControl(String command) {
    int state = command.toInt();
    vibrating = state > 0 ? 1 : 0;
}

void flipOnBoardLedState() {
    onBoardLedState = onBoardLedState <=0 ? 1 : 0;
    digitalWrite(PIN_BOARD_LED, onBoardLedState);
    digitalWrite(PIN_VIBRATOR, onBoardLedState <= 0 ? 1 : 0);
}