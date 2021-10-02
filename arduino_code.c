/**********************************************************************************
 *  TITLE: Blynk + Manual Switch control Relays using Multiple NodeMCU (Real time feedback + no WiFi control)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/v4MealVM5bE
 *  Related Blog : https://iotcircuithub.com/esp8266-projects/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board ESP8266 NodeMCU : https://github.com/esp8266/Arduino
 *  Download the libraries
 *  https://github.com/blynkkk/blynk-library/releases/download/v0.6.1/Blynk_Release_v0.6.1.zip
 **********************************************************************************/

//#define BLYNK_PRINT Serial            
#include <BlynkSimpleEsp8266.h>

#define wifiLed   16   //D0

const int relayPins[] = {5, 4, 14, 12, 6};
const int switchPins[] = {10, 0, 13, 3, 7};
bool toggleStates[] = {true, true, true, true, true};

const int vpinButtons[] = {V1, V2, V3, V4, V5};

int wifiFlag = 0;

#define AUTH "Auth-token-blynk"                 // You should get Auth Token in the Blynk App.  
#define WIFI_SSID "Karthik Mothiki"             //Enter Wifi Name
#define WIFI_PASS "11112002"         //Enter wifi Password

BlynkTimer timer;

/// Function to toggle the relay based on current toggle state for it
void relayOnOff(int relay){
    if (toggleStates[relay - 1]) {
        digitalWrite(relayPins[relay - 1], LOW);
        toggleStates[relay - 1] = false;
        Serial.print("Device");
        Serial.print(relay);
        Serial.println(" ON");
    } else {
        digitalWrite(relayPins[relay - 1], HIGH);
        toggleStates[relay - 1] = true;
        Serial.print("Device");
        Serial.print(relay);
        Serial.println(" OFF");
    }
    delay(100);
}

/// Switch Control based on whether operation is being run with internet or not.
void switch_control(bool withInternet = false){
  for (int i = 0; i < sizeof(switchPins) / sizeof(switchPins[0]); ++i) {
    if (digitalRead(switchPins[i]) == LOW){
      delay(200);
      relayOnOff(i + 1); 
      if (withInternet) Blynk.virtualWrite(vpinButtons[i], toggleStates[i]);   // Update Button Widget  
    }
  }
}

/// When App button is pushed - switch the state
BLYNK_WRITE(VPIN_BUTTON) {
  toggleState[VPIN_BUTTON - 1] = param.asInt();
  digitalWrite(relayPins[VPIN_BUTTON - 1], toggleStates[VPIN_BUTTON - 1]);
}

/// Called every 3 seconds by SimpleTimer to check Blynk Status
void checkBlynkStatus() {
  if (Blynk.connected() == false) wifiFlag = 1;
  else {
    wifiFlag = 0;
    Blynk.virtualWrite(vpinButtons[0], toggleStates[0]);
  }
}

void setup() {
  Serial.begin(9600);

  // Wifi LED setup
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);

  // Switch pins, relay pins setup
  for (int i = 0; i < sizeof(relayPins) / sizeof(relayPins[0]); ++i) {
    // Set up pinmodes
    pinMode(relayPins[i], OUTPUT);
    pinMode(switchPins[i], INPUT_PULLUP);

    //During Starting all Relays should TURN OFF
    digitalWrite(relayPins[i], toggleStates[i]);
  }

  // Set up WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  timer.setInterval(3000L, checkBlynkStatus); // check if Blynk server is connected every 3 seconds
  Blynk.config(AUTH);
  delay(1000);

  // Set up Blynk virtual buttons
  for (int i = 0; i < sizeof(vpinButtons) / sizeof(vpinButtons[0]); ++i) {
    Blynk.virtualWrite(vpinButtons[i], toggleStates[i]);
  }
}

void loop() {  
  if (WiFi.status() != WL_CONNECTED) {
    //Serial.println("WiFi Not Connected");
    digitalWrite(wifiLed, HIGH);
  }
  else {
    //+Serial.println("WiFi Connected");
    digitalWrite(wifiLed, LOW); //Turn on WiFi LED
    Blynk.run();
  }

  timer.run(); // Initiates SimpleTimer
  switch_control(wifiFlag == 0);
}
