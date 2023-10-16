#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

long lastSendTime = 0;        // last send time

String Local_ID;

int interval = 5000;          // interval 0between sends
#define timeSecond 30
unsigned long now = millis();
unsigned long lasttrigger = now;
boolean starttimer = true;

String message = "";
char char_received ;

void setup() {
  Serial.begin(115200);                   // initialize serial

  SerialBT.begin("elephant1"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

}

void loop() {
  
  if (SerialBT.available()) {
    char char_received = SerialBT.read();
    if (char_received != '\n') {
      message += String (char_received);
    } else {
      Serial.println(message);
      ///////////////////////////////////////////////////////
      /////////////////ส่งข้อความบอกมาสเตอร์ว่า ON$OK ///////////
      ///////////////////////////////////////////////////////      
//      if(){//ข้อความที่รับค่าเข้ามาว่าส่งค่าอะไรมา
//        ;
//      }
//      else if(){
//        ;
//      }
//      else if(){
//        ;
//      }
    }
//    Serial.println(message);
  }
  
  now = millis();
  if(starttimer == true && (now - lasttrigger > (timeSecond * 1000))){
    if(message == ""){
      Serial.println("Bluetooth Lost");
      starttimer = false;
      ///////////////////////////////////////////////////////
      /////////////////ส่งข้อความบอกมาสเตอร์ว่า ON$LOST ///////////
      ///////////////////////////////////////////////////////
      ESP.restart();
    }else {
      Serial.print("message timer finish = ");Serial.println(message);
      starttimer = true;
      lasttrigger = millis();
      message = "";
    }
  }
}
