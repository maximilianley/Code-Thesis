#include "sensor_logik.h"
#include "LoRa.h"
#include "SPI.h"
#include "wifi_connection_handler.h"
#include "http_handler.h"
#include "time_assist.h"

#include <Wire.h>
#include <BH1750.h>

#define BAND 868E6

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  1        /* Time ESP32 will go to sleep (in seconds) */

BH1750 lightMeter, lightMeter2;

// Clock-Drift
const int maxOfflineMeasurements = 100; // <- diese Konstante korrekt wählen
RTC_DATA_ATTR int clockDriftMeasurementsCount = 0;

// Iterationszähler
const int partialMeasurementAmount = 3;// 4?; // <- letzte ist nur fuer JSON Post
RTC_DATA_ATTR int partialMeasurementCount = 0;

// Tiefschlaf-Dauer
const unsigned long metaWakeup = 2;
const unsigned long shortWakeup = 5;
const unsigned long longWakeup = 10;

// Verbindungsaufbau
const int connectionAttemptsAmount = 4;
RTC_DATA_ATTR int connectionAttemptsCount = 0;

RTC_DATA_ATTR String heldMessage = "";

void setup() {
  Serial.begin(115200);
  
  Wire1.begin(21, 22);
  Wire.begin(4, 15);

  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire);
  lightMeter2.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1);

  float lux1 = messenLux(lightMeter);
  float lux2 = messenLux(lightMeter2);

  Serial.println("Messungen:");
  Serial.println(lux1);
  Serial.println(lux2);

  boolean isIteration = false;
  boolean gotTimestamp = false;
  unsigned long sleep_millis_amount;
  unsigned int sleep_seconds_amount = 1;
  
  boolean conn = establishConnection();
  if (conn){

    getLocalTime();
    if ( ( (this_second > -1) && (this_second < 60) ) &&
         ( (this_minute > -1) && (this_minute < 60) ) &&
         ( (this_hour > -1) && (this_hour < 24) ) )
    {
          Serial.println("Timestamp acquired.");
          if ( partialMeasurementCount >= (partialMeasurementAmount-1) ){
            clockDriftMeasurementsCount = 0; // Clockdrift wird auf 0 zurückgesetzt
            gotTimestamp = true;
            elapsedMillis(); // Timer hier beginnen
          }
          isIteration = true;
    }

    if ( partialMeasurementCount >= (partialMeasurementAmount-1) ){
      postJSON("/waldsensor", heldMessage); // <-- benutze die Nachricht die du vorher generiert hast
    }

    conclude(); // Zeitstempel speichern
    closeConnection();

    connectionAttemptsCount = 0;
  }else{
    connectionAttemptsCount++;
  }

  if (connectionAttemptsCount >= connectionAttemptsAmount){
    isIteration = true;
  }

  if (isIteration){

    // pro Iteration eine Messung
    int code = getMeasurementCode(lux1, lux2);
    String message = getMessage(code);
    heldMessage = message; 
      
    partialMeasurementCount += 1;
    if (clockDriftMeasurementsCount >= maxOfflineMeasurements){
      float luxRef = messenLux(lightMeter);
      float luxDown = messenLux(lightMeter2);
      int code = getMeasurementCode(luxRef, luxDown);
      // Da der Clockdrift hier schon zu gross ist, ist die Messung nicht mehr verlaesslich
    }
    connectionAttemptsCount = 0;
  
    // wakeup fuer Deep Sleep setzen, also use elapsedMillis()
    if (partialMeasurementCount >= partialMeasurementAmount){
      if (gotTimestamp){
        // Schlafdauer bis 12:00 Uhr des nächsten Tages berechnen
        int remainingSecondsToMidnight = (23 - lastHour) * 3600 + (59 - lastMinute) * 60 + (59 - lastSecond);
        int remainingSeconds = remainingSecondsToMidnight + 12 * 3600;
        //int long_sleep_millis = remainingSeconds * 1000;
        //sleep_millis_amount = long_sleep_millis;
        sleep_seconds_amount = remainingSeconds;
      }else{
        // ESP ist nicht synchronisiert
        clockDriftMeasurementsCount++;
        sleep_seconds_amount = longWakeup;
      }
      partialMeasurementCount = 0;
    }else{
      sleep_seconds_amount = shortWakeup;
    }
  }else{
    sleep_seconds_amount = metaWakeup;
  }
  
  Serial.println("Boot counter: "+ String(bootCount));
  bootCount++;
  esp_sleep_enable_timer_wakeup(sleep_seconds_amount * uS_TO_S_FACTOR);

  Serial.println("Going to sleep now");
  delay(2000);
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // put your main code here, to run repeatedly:
}
