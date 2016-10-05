// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <aREST.h>

// PINS
#define PIN_BOARD_LED 0

// Create aREST instance
aREST rest = aREST();

// WiFi parameters
const char* ssid = "vibrator";
const char* password = "";

// The port to listen for incoming TCP connections
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Declare functions to be exposed to the API
int rythmControl(String command);
int speedControl(String command);

// Variables
int rythmState = 0;
int onBoardLedState = 0;
float vibrateSpeed = 1;

void setup(void)
{
    // Start Serial
    Serial.begin(115200);

    // Init variables and expose them to REST API
    //rest.variable("on_board_led", &onBoardLedState);

    // Function to be exposed
    rest.function("rythm", rythmControl);
    rest.function("speed", speedControl);

    // Give name & ID to the device (ID should be 6 characters long)
    rest.set_id("1");
    rest.set_name("iot_prototype");

    // Connect to WiFi
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
}

void loop() {
    handleREST();
    rythm();
}

void handleREST() {
    WiFiClient client = server.available();
    if (client && client.available()) {
        rest.handle(client);
    } else {
        delay(1);
    }
}

void rythm() {
    if(rythmState > 0) {
        float delayQ = (2 - vibrateSpeed);

        switch(rythmState) {
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
}

int rythmControl(String command) {
    int state = command.toInt();
    rythmState = state;
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

void flipOnBoardLedState() {
    if(onBoardLedState == 0) {
        onBoardLedState = 1;
    } else {
        onBoardLedState = 0;
    }

    digitalWrite(PIN_BOARD_LED, onBoardLedState);
}