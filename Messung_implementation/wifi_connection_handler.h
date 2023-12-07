#include <WiFi.h>

const char* ssid = "FRITZ!Box 7490";//"Maxi";//"FRITZ!Box Gastzugang";//"Maxi";
const char* password = "02084253628004701733";//"2222222223";//"yzupwu5nvcaK";//"2222222223";

boolean establishConnection() {
  //connect to WiFi
  int attempts = 0;
  int maxAttempts = 4;
  boolean established = false;
  
  while(!established && (attempts < maxAttempts)){
    Serial.printf("Connection attempt %d \n", attempts);
    Serial.printf("Connecting to %s \n", ssid);
    WiFi.begin(ssid, password);
    
    int stepDelay = 500;
    int maxWait = 12*stepDelay;
    int delayCount = 0;
    
    while (!established && (delayCount < maxWait)) {
        delay(stepDelay);
        Serial.print(".");
        established = (WiFi.status() == WL_CONNECTED);
        delayCount += stepDelay;
    }
    attempts++;
  }

  if (established){
    Serial.println(" CONNECTED");
    return true;
  }else{
    Serial.println(" Could not connect to network.");
    return false;
  }
  
}

void closeConnection(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
