

// Required includes for NFC scanner
#include <SPI.h>
#include <MFRC522.h>

// Pins for NFC scanner

const int firstSdaPin = 53;
const int firstRstPin = 5;

// Pins for UDS

const int trigPin = 6;
const int echoPin = 7;

// Pins for LEDS

const int lightRelayPin = A7;

// Pins for lock

const int lockRelayPin = A5;

// Create instance of MFRC522

MFRC522 mfrc(firstSdaPin, firstRstPin);

// Define variables to be used by UDS

float duration, distance;

// Define variables to be used for the puzzle logic

bool nfcCompleted;
bool udsCompleted;
bool doorUnlocked;

// Define variables which hold the correct NFC tags

String nfcTag = "D3 A8 F9 11";

// Define mode for each of the pins
void setupPinModes() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(lockRelayPin, OUTPUT);
  pinMode(lightRelayPin, OUTPUT);
}

void setup() {
  setupPinModes();

  // Initialise serial communication
  Serial.begin(9600);

  // Initialise SPI bus
  SPI.begin();

  // Initliase MFRC522 
  mfrc.PCD_Init();
}

void loopNFC()
{
  // Return if no card present or serial is unreadable
  if(!mfrc.PICC_IsNewCardPresent() || !mfrc.PICC_ReadCardSerial())
  {
    return;
  }

  // Define variable to store the tag's ID
  String nfcContent = "";

  for(byte i = 0; i < mfrc.uid.size; i++)
  {
    nfcContent.concat(String(mfrc.uid.uidByte[i] < 0x10 ? " 0" : " "));
    nfcContent.concat(String(mfrc.uid.uidByte[i], HEX));
  }

  // Transform tag's ID to uppercase
  nfcContent.toUpperCase();

  Serial.println(nfcContent);

  // Check if ID is equal to correct tag's ID
  if(nfcContent.substring(1) == nfcTag)
  {
    // Set completed variable to true
    nfcCompleted = true;

    // Turn on the ultraviolet lights
    digitalWrite(lightRelayPin, HIGH);
    return;
  }

  // Delay between reading again
  delay(100);
}

void loopUDS()
{
  // Sequence to measure distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(trigPin, LOW);

  // Get pulse in
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance from duration

  distance = (duration * 0.343) / 2;

  Serial.println(distance);

  // Check if distance is within correct distance
  if(distance <= 1500 and distance >= 1000)
  {
    // Turn ultra violet lights off
    digitalWrite(lightRelayPin, LOW);

    // Set UDS to completed
    udsCompleted = true;
    return;
  }

  // Delay before reading again
  delay(100);
}

void unlockDoor()
{
  // Write HIGH to door relay to unlock it
  digitalWrite(lockRelayPin, HIGH);
}

// Loop through each aspect of game until they are all completed
void loop() {
  if(!nfcCompleted)
  {
    loopNFC();
    return;
  }

  if(!udsCompleted)
  {
    loopUDS();
    return;  
  }

  if(!doorUnlocked)
  {
     unlockDoor();
     return;
  }
}
