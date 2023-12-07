#include "sensor_logik.h"
//#include "LoRa.h"
#include "SPI.h"
#include "wifi_connection_handler.h"
#include "http_handler.h"
#include "time_assist.h"

#include <Wire.h>
#include <BH1750.h>
#include "heltec.h"

#define BAND 868E6

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */

BH1750 lightMeter, lightMeter2;

// Variablen fuer den Messzustand
const int maxOfflineMeasurements = 100; // <- diese Konstante korrekt wählen
RTC_DATA_ATTR int clockDriftMeasurementsCount = 0;

const int partialMeasurementAmount = 3;// 4?; // <- letzte ist nur fuer JSON Post
RTC_DATA_ATTR int partialMeasurementCount = 0;

const unsigned long metaWakeup = 2;
const unsigned long shortWakeup = 5;
const unsigned long longWakeup = 10;
RTC_DATA_ATTR unsigned long nextWakeupDuration = 10;
// zu einer korrekten Uhrzeit aufsetzen/initialisieren

// Variablen fuer Persistenz ueber DeepSleep
const int connectionAttemptsAmount = 4;
RTC_DATA_ATTR int connectionAttemptsCount = 0;

bool ZZT = false;
void setup() {
  Heltec.begin(true /*DisplayEnable enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
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
  
  int code = getMeasurementCode(lux1, lux2);
  LoRa.beginPacket();
  LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);
  LoRa.print(String(code));
  LoRa.endPacket();

  // sleep_millis_amount = longWakeup;
  // sleep_millis_amount = shortWakeup;
  sleep_millis_amount = metaWakeup;
  
  Serial.println("whats up "+ String(bootCount));
  bootCount++;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("Going to sleep now");
  delay(2000);
  Serial.flush();
  esp_deep_sleep_start();
}

void loop() {
  // put your main code here, to run repeatedly:

}
