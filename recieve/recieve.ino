#include <RFM69.h>
#include <SPI.h>

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 9600
#define ACK_TIME    30  // # of ms to wait for an ack

RFM69 radio;

typedef struct {		
  int nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
} Payload;
Payload theData;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.println("Serial started");
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
  char buff[50];
  sprintf(buff, "\nListening at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
}

byte ackCount=0;

void loop() {
  if (radio.receiveDone()) {
    if(radio.DATALEN != sizeof(Payload)) {
      Serial.print("Invalid payload received, not matching Payload struct!");
    }
    else {
      theData = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
        Serial.print("ID: ");
        Serial.print(theData.nodeId);
        Serial.print(", TIME: ");
        Serial.print(theData.uptime);
        Serial.print(", RSSI: ");
        Serial.print(radio.readRSSI());
        Serial.println();
    }
    if (radio.ACKRequested()) {
      ACKsend();
    }
    Blink(LED,100);
  }
}
void ACKsend(){
  //  byte theNodeID = radio.SENDERID;
  radio.sendACK();
  //Serial.print(" - ACK sent.");
  // When a node requests an ACK, respond to the ACK
  // and also send a packet requesting an ACK (every 3rd one only)
  // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
  if (ackCount++%3==0) {
    //   Serial.print(" Pinging node ");
    //Serial.print(theNodeID);
    //Serial.print(" - ACK...");
    delay(3); //need this when sending right after reception .. ?
    //    if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
    // Serial.print("ok!");
    // else Serial.print("nothing");
  }
}
void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}