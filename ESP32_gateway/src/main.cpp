#include "EspMQTTClient.h"
#include "ArduinoJson.h"
#include "HardwareSerial.h"
#include <Arduino.h>

String robotState = "IDLE";
const char* newTimeStamp = "";
const char* oldTimeStamp = "";
StaticJsonDocument<1000> recipe;

EspMQTTClient client(
  "nick",
  "12345678",
  "test.mosquitto.org",  // MQTT Broker server ip (test.mosquitto.org)
  "",   // Can be omitted if not needed
  "",   // Can be omitted if not needed
  "",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void publishMsg()
{
  client.publish("BIP/TechConnect/State", robotState);
}

void onReceive(String msg)
{
  // Send the received JSON via Serial
  Serial.println(msg);
}

void setup()
{
  Serial.begin(115200);
  // Optional functionalities of EspMQTTClient:
  //client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  // client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridden with enableHTTPWebUpdater("user", "password").

}

// This function is called once everything is connected (WiFi and MQTT)
// WARNING: YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  Serial.print("connected\n");
  client.subscribe("BIP/TechConnect/Pill/Receive", onReceive);
}

void loop()
{
  client.loop();
}
