#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

bool newDataReceived = false;
std::string receivedData;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Device connected");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Device disconnected");
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        newDataReceived = true;
        receivedData = value;
      }
    }
};

const int nssPin = 5;          // LoRa radio chip select
const int rstPin = 15;       // LoRa radio reset
const int dio0Pin = 2;         // change for your board; must be a hardware interrupt pin

String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages
//byte localAddress = 0xA1;     // address of this device
String localAddress = "A1";
byte destination = 0xAB;      // destination to send to
long lastSendTime = 0;        // last send time

String Local_ID;
String received;
int interval = 10000;          // interval 0between sends

void setup() {
  Serial.begin(115200);                   // initialize serial
  // Create the BLE server
  BLEDevice::init("ESP32_BLE_Server");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE service
  pService = pServer->createService(SERVICE_UUID);

  // Create the BLE characteristic
  pCharacteristic = pService->createCharacteristic(
                      BLEUUID(CHAR_UUID),
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  pCharacteristic->setCallbacks(new MyCallbacks());

  // Add a descriptor for notifications
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(BLEUUID(SERVICE_UUID));
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // no preference
  BLEDevice::startAdvertising();
  while (!Serial);

  Serial.println("LoRa Duplex with callback Test1");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(nssPin, rstPin, dio0Pin);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa_1 init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.setSyncWord(0xAB);
  LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("LoRa_1 init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    String msg;
    String message;// = "Hi. I'm LoRa_1";   // send a message
    msgCount++;                           // increment message ID
    if (newDataReceived) {
      String received = receivedData.c_str();
      Serial.print("BLE Received Data: ");
      Serial.println(received); //ON$OK or detected
      newDataReceived = false;
    }
    int pos0 = received.indexOf('$');
    if (received.substring(pos0 - 1, pos0 - 2) == "O") {
      msg = "slave1";
      message = String(msgCount) + "@" +
                String(localAddress) + "/" +
                String(msg) + "&" +
                String(received);
    // Sending 1@A1/slave1&ON$OK
    } else if (received.length() == 7) {
      msg = "cam1";
      message = "@" +String(localAddress) + "/" +
                String(msg) + "&" +
                String(received);
    // Sending @A1/cam1&DETECTED
    } else {
      msg = "slave1";
      message = String(msgCount) + "@" +
                String(localAddress) + "/" +
                String(msg) + "&No cmd from bluetooth";
    // Sending 1@A1/slave1&No cmd from bluetooth
    }
    sendMessage(message);
    Serial.println("Sending " + message); Serial.println();
    lastSendTime = millis();            // timestamp the message
    interval = random(9000) + 1000;     // 2-3 seconds
    //    LoRa.receive();                     // go back into receive mode
  }
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();
  LoRa.print(outgoing);
  LoRa.endPacket();
  LoRa.receive();
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  Serial.print("Received packet '");
  // read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&humidity$light#batterylevel
    // String example: 1/27.43&654$555#95.34
    Serial.print(LoRaData);
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.print(LoRa.packetRssi());
    Serial.print(", Snr: ");
    Serial.println(LoRa.packetSnr());

    int pos1 = LoRaData.indexOf('@');
    int pos2 = LoRaData.indexOf('/');
    Local_ID = LoRaData.substring(pos1 + 1, pos2);

    if (Local_ID == "A2") {
      Serial.println("LoRa ID 'A2'");
      int pos3 = LoRaData.indexOf('&');
      int pos4 = LoRaData.indexOf('$');
      int pos5 = LoRaData.indexOf('#');

      String LoRa_A2_str = LoRaData.substring(pos2 + 1, LoRaData.length());
      Serial.print("Data Lora_A2 onReceive : "); Serial.println(LoRa_A2_str);
      Serial.println();
    }
    else {
      Serial.println("error Local_ID onReceive not use");
      Serial.println();
    }
  }
}
