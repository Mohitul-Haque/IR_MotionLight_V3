#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>


// Connect to the WiFi
const char* ssid = "DataSoft_WiFi";
const char* password = "support123";
const char* mqtt_server = "test.mosquitto.org";
const int mqttPort = 1883;
String did = "RA101";


//mqtt
unsigned long lastReconnectTime = 0;
WiFiClient espClient;
PubSubClient client(espClient);

// constants won't change. They're used here to set pin numbers:
const int buttonPin = D5;    // the number of the pushbutton pin
const int ledPin = D6;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);


     setup_wifi();

    //For MQTT
   client.setServer(mqtt_server, mqttPort);
   client.setCallback(callback);
}

void loop() {

   setMqttReconnectTimer();

      
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
//        ledState = !ledState;
         ledState = HIGH;
         Serial.println("STATE HIGH");
         int res = client.publish("motion/response", "OFF");
//         delay(100);
//         Serial.println("Result: "+res);
      }
      else{
         ledState = LOW;
         Serial.println("STATE LOW");
         int res = client.publish("motion/response", "ON");
//         delay(100);
//         Serial.println("Result: "+res);
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}



void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setMqttReconnectTimer(){
  if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
        lastReconnectTime = now;
        Serial.println("Ticking every 5 seconds");
        // Attempt to reconnect
        if (reconnect()) {
          lastReconnectTime = 0;//GOOD
        }
      }
   }else{
      client.loop();
   }   
}

