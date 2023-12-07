/* 
  Check the new incoming messages, and print via serialin 115200 baud rate.
  
  by Aaron.Lee from HelTec AutoMation, ChengDu, China
  成都惠利特自动化科技有限公司
  www.heltec.cn
  
  this project also realess in GitHub:
  https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series
*/

#include "heltec.h"
#include "SPI.h"
#include "http_handler.h"
#include "wifi_connection_handler.h"
#include "events.h"

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
void setup() {
    //WIFI Kit series V1 not support Vext control
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

}

void loop() {

  if (isConnected()){

    LoRa.setTimeout(1000);
    //Serial.println("Start receiving.");
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      // received a packet
      Serial.print("Received packet '");
      // read packet
      String receivedData = "";
      while (LoRa.available()) {
        receivedData += (char)LoRa.read();
      }
      Serial.print(receivedData);
      // print RSSI of packet
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());

      // get and post meassage
      int message_code = receivedData.toInt();
      String message = getMessage(message_code);
      if (message != NONE_MESSAGE){
        Serial.print("Posting message:");
        Serial.print(message);
        postJSON("/waldsensor", message);
      }else{
        Serial.print("Invalid code.");
      }
    }
  }else{
    Serial.println("Closing connection");
    closeConnection();
    Serial.println("Establishing connection");
    establishConnection();
  }
}
