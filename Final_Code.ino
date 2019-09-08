#include <MKRWAN.h>


//logic variables
int reed = 2;
bool curr_state = 0, prev_state = 0;
int reads = 0;
//LoRa Variables
const char *appEui = "70B3D57ED00202D3";
const char *appKey = "93A39E77524C571BC04A17B53067BAAD";
_lora_band region = US915;
LoRaModem modem(Serial1);

// the setup routine runs once when you press reset:
void setup() {
  // initialize pins
  Serial.begin(115200);
  pinMode(reed, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,0);
  //LoRa Setup
  while (!Serial);
  if (!modem.begin(region)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());
  int connected = modem.joinOTAA(appEui, appKey);
    while (!connected) {
      Serial.println("Something went wrong; waiting 1 min...");
      delay(1000*60);
      Serial.println("Retrying...");
      Serial.print("Your device EUI is: ");
      Serial.println(modem.deviceEUI());
      connected = modem.joinOTAA(appEui, appKey);
    }
  digitalWrite(LED_BUILTIN,1);
  Serial.println("Successfully joined the network!");
  Serial.println("Enabling ADR and setting low spreading factor");
  modem.setADR(true);
  modem.dataRate(5);
  
  //initialize
  update();
  Serial.print("Sending first message...");
  send(curr_state);
  //send_tb(curr_state);
}
// the loop routine runs over and over again forever:
void loop() {
  //update state variables
  update();
  // edge triggered logic
  if (prev_state == 1) { //door was closed
    if (curr_state == 0)  //send that the door has opened
      send(curr_state);
      //send_tb(curr_state);//test bench
    //else                //do nothing
  }
  else { //door was opened
    if (curr_state == 1)  //send that the door has been closed
      send(curr_state);
      //send_tb(curr_state);//test bench
    //else                //do nothing
  }

}

void send(bool data) {
  Serial.println("Checking samples consistency...");
  for (int i = 0 ; i < 10 ; i++) {
    update();
    if (curr_state != data) {
      Serial.println("Nothing was sent due to inconsistent samples.");
      return;
    }
  }

  modem.beginPacket();
  modem.print(data);
  int err = modem.endPacket(false);
  if (err > 0) {
    Serial.println("****************");
    Serial.print(data);
    Serial.println(" has been sent!");
    Serial.println("****************");
  } else {
    Serial.println("****************");
    Serial.println("Error sending data");
    Serial.println("****************");
  }
  Serial.println("Waiting for 1 min...");
  digitalWrite(LED_BUILTIN,0);
  delay(1000 * 60);
  digitalWrite(LED_BUILTIN,1);
  Serial.println("Reading Samples...");
}

void send_tb(bool data) {
  Serial.println("Checking samples consistency...");
  for (int i = 0 ; i < 10 ; i++) {
    update();
    if (curr_state != data) {
      Serial.println("Nothing was sent due to inconsistent samples.");
      return;
    }
  }
  Serial.println("****************");
  Serial.print(data);
  Serial.println(" has been sent!");
  Serial.println("****************");
  Serial.println("Waiting for 1 sec...");
  digitalWrite(LED_BUILTIN,0);
  delay(1000);
  digitalWrite(LED_BUILTIN,1);
  Serial.println("Reading Samples...");
}

void update() {
  prev_state = curr_state;
  curr_state = digitalRead(reed);
  if (prev_state !=  curr_state) {
    Serial.print("Current State = ");
    Serial.println(curr_state);
  }
  delay(100);
}
