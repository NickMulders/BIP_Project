
#include "ArduinoJson.h"

#include <morobot.h>
#if USE_DABBLE != 0
  #include <dabble_include.h>
#endif

#define bufferLength 256
#define MOROBOT_TYPE   morobot_s_rrp
#define MOROBOT_SERIAL_PORT    "Serial1"

MOROBOT_TYPE morobot;

String robotState = "";
String receivedData = "";
StaticJsonDocument<1000> recipe;

bool isBusy = false;

struct Pill
{
  String Color;
  String Shape;
};

// Define the pills
Pill MondayPill;
Pill TuesdayPill;
Pill WednesdayPill;
Pill ThursdayPill;
Pill FridayPill;
Pill SaturdayPill;
Pill SundayPill;

// Positions of pills
// To be completed...
long Yellow_Round[3] = {-56, -89.0, 0.0};
long Yellow_Oval[3] = {-44, -92, 0.0};
long Yellow_Square[3] = {-65, -90, 0};
long Grey_Round[3] = {-44, -72, 0};
long Grey_Oval[3] = {-34, -72, 0};
long Grey_Square[3] = {-47, -63, 0};
long Dropping_Basket[3] = {9, -86, 0};

int Yellow_Round_picked = 0;
int Yellow_Oval_picked = 0;
int Yellow_Square_picked = 0;
int Grey_Round_picked = 0;
int Grey_Oval_picked = 0;
int Grey_Square_picked = 0;
int Dropping_Basket_picked = 0;


void InitPill(const String& color, const String& shape, Pill& pill)
{
  pill.Color = color;
  pill.Shape = shape;
}

void MoveRobot(long* pillPosition, int* pill_picked)
{
   morobot.setSpeedRPM(3);
   morobot.moveHome();

   // Move to pill location
   morobot.moveAngles(pillPosition);
   morobot.waitUntilIsReady();

   // pick up pill
   float moveAmount = -10 + ((*pill_picked) * -5);
   (*pill_picked)++;
   morobot.moveInDirection('z', moveAmount);  
   delay(4000);
   digitalWrite(24, HIGH);
   delay(1500);
   morobot.moveInDirection('z', (moveAmount/-1));  
   delay(3000);
   
   // Move to dropping basket
   morobot.moveHome();
   morobot.waitUntilIsReady();
   morobot.moveAngles(Dropping_Basket);
   morobot.waitUntilIsReady();

   // Release
   delay(1000);
   digitalWrite(24, LOW);
   delay(3000); 

   // Move to home
   morobot.moveHome();
   morobot.waitUntilIsReady();
}


void DeterminePillLocation(Pill pill)
{
  if (pill.Color == "Yellow" && pill.Shape == "Round")
  {
    Serial.println("Yellow - Round");
    MoveRobot(Yellow_Round, &Yellow_Round_picked);
  }
  else if (pill.Color == "Yellow" && pill.Shape == "Oval")
  {
    Serial.println("Yellow - Oval");
    MoveRobot(Yellow_Oval, &Yellow_Oval_picked);
  }
  else if (pill.Color == "Yellow" && pill.Shape == "Square")
  {
    Serial.println("Yellow - Square");
    MoveRobot(Yellow_Square, &Yellow_Square_picked);
  }
    if (pill.Color == "Grey" && pill.Shape == "Round")
  {
    Serial.println("Grey - Round");
    MoveRobot(Grey_Round, &Grey_Round_picked);
  }
  else if (pill.Color == "Grey" && pill.Shape == "Oval")
  {
    Serial.println("Grey - Oval");
    MoveRobot(Grey_Oval, &Grey_Oval_picked);
  }
  else if (pill.Color == "Grey" && pill.Shape == "Square")
  {
    Serial.println("Grey - Square");
    MoveRobot(Grey_Square, &Grey_Square_picked);
  }
}

void processReceivedData(String msg)
{
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error)
  {
    Serial.print("Deserialization error: ");
    Serial.println(error.c_str());
    return;
  }
  
  recipe = doc;

  // Init the pills and move to the correct location
  InitPill(doc["Monday"]["color"], doc["Monday"]["shape"], MondayPill);
  DeterminePillLocation(MondayPill);

  InitPill(doc["Tuesday"]["color"], doc["Tuesday"]["shape"], TuesdayPill);
  DeterminePillLocation(TuesdayPill);

  InitPill(doc["Wednesday"]["color"], doc["Wednesday"]["shape"], WednesdayPill);
  DeterminePillLocation(WednesdayPill);

  InitPill(doc["Thursday"]["color"], doc["Thursday"]["shape"], ThursdayPill);
  DeterminePillLocation(ThursdayPill);

  InitPill(doc["Friday"]["color"], doc["Friday"]["shape"], FridayPill);
  DeterminePillLocation(FridayPill);

  InitPill(doc["Saturday"]["color"], doc["Saturday"]["shape"], SaturdayPill);
  DeterminePillLocation(SaturdayPill);

  InitPill(doc["Sunday"]["color"], doc["Sunday"]["shape"], SundayPill);
  DeterminePillLocation(SundayPill);
}



void setup()
{
  Serial.begin(115200);

  pinMode(24, OUTPUT);

  morobot.begin(MOROBOT_SERIAL_PORT);
  morobot.releaseBreaks();
  Serial.println("You have 5 seconds to set the robot home position");
  delay(5000);
  morobot.setZero();        // Set the axes zero when move in  
  morobot.moveHome();       // Move the robot into initial position 
  morobot.waitUntilIsReady();
}


void loop()
{
//    morobot.releaseBreaks();
//    Serial.println("motor 0 pos: " + String(morobot.getActAngle(0)));
//    Serial.println("motor 1 pos: " + String(morobot.getActAngle(1)));
//    Serial.println("motor 2 pos: " + String(morobot.getActAngle(2)));
//    delay(2000);
  if (!isBusy)
  {
    if (Serial.available() > 0) { // Check if data is available to read
      receivedData = Serial.readString(); 
      // Process the received data
      receivedData.replace("\n", "");
      receivedData.trim();
      isBusy = true;
    }
  }
  else
  {
   processReceivedData(receivedData);
   isBusy = false;
  }
}
