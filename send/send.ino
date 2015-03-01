#include <RFM69.h>
#include <SPI.h>

#define NODEID      10 //node 0 is broadcast
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_433MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         "thisIsEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         9
#define SERIAL_BAUD 9600
#define ACK_TIME    50  // # of ms to wait for an ack
#define NB_ATTEMPTS_ACK 5 //number of attempts to try before giving up

int TRANSMITPERIOD = 2000; //transmit a packet to gateway so often (in ms) (one minute)
boolean requestACK = true;
RFM69 radio;

typedef struct {		
    int nodeId; //store this nodeId
    unsigned long uptime; //uptime in ms
} Payload;
Payload payload;

void setup() {
    Serial.begin(SERIAL_BAUD);
    radio.initialize(FREQUENCY,NODEID,NETWORKID);
    radio.setHighPower(); //uncomment only for RFM69HW!
    //radio.setPowerLevel(20); //Set power level for Radio.
    radio.encrypt(KEY);
    radio.sleep();
    char buff[50];
    sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
}

long lastPeriod = -1;

void loop() {
    int nAttempt = 0; //number of sends attempted
    bool flag_ACK_received = false; //is the acknowledgement recieved?

    payload.nodeId = NODEID;
    payload.uptime = millis()/1000;

    while(nAttempt < NB_ATTEMPTS_ACK && !flag_ACK_received) { //while ACK not received and # of retrys not exceeded
        if (radio.sendWithRetry(GATEWAYID, (const void*)(&payload), sizeof(payload))){ //Send payload
            Serial.print(" ok!");
            flag_ACK_received = true; //if ACK received, all OK
            Blink(LED, 100);
        } else {
            Serial.print("- Failed, ");
            Serial.println("sending again...");
            radio.sendWithRetry(GATEWAYID, (const void*)(&payload), sizeof(payload)); //try sending again
            nAttempt++; 
            delay(500);
        }
    }
    delay(100);
    radio.sleep();
    delay(TRANSMITPERIOD);
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,LOW);
  delay(DELAY_MS);
  digitalWrite(PIN,HIGH);
}