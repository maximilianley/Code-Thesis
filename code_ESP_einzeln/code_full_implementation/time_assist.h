#include "time.h"

// Variablen zum ABrufen des letzten Zeitstempels
RTC_DATA_ATTR int lastSecond = 99;
RTC_DATA_ATTR int lastMinute = 99;
RTC_DATA_ATTR int lastHour = 99;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

int this_second = -1;
int this_minute = -1;
int this_hour = -1;

boolean time_valid = false;
struct tm timeinfo;

// for local time measurement
unsigned long timer_point = millis();

// diese Methode sollte nur einmal pro Wakeup aufgerufen werden
void getLocalTime(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(&timeinfo, "%H:%M:%S");
  
  char csecond[3];
  char cminute[3];
  char chour[3];
  
  strftime(csecond, 3, "%S", &timeinfo);
  strftime(cminute, 3, "%M", &timeinfo);
  strftime(chour, 3, "%H", &timeinfo);

  this_second = atoi(csecond);
  this_minute = atoi(cminute);
  this_hour = atoi(chour);

  time_valid = true;
}

// laeuft seit startup
unsigned long elapsedMillis(){
  unsigned long difference = millis() - timer_point;
  timer_point = millis();
  return difference;
}

void conclude(){
  lastSecond = this_second;
  lastMinute = this_minute;
  lastHour = this_hour;
}
