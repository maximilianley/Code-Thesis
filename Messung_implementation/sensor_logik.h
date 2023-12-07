#include "events.h"
#include <BH1750.h>

RTC_DATA_ATTR int bootCount = 0;

int maehenArrLength = 10;
RTC_DATA_ATTR bool maehenArr[10] = {false, false, false, false, false, false, false, false, false, false};

float expectedMaxStep = 0.02f;
RTC_DATA_ATTR float remainingOverflow = 0.0f;
RTC_DATA_ATTR float lastHeightFactor = 10.0f; // gross initialisieren, fuer den erstesn Wert

int overflowMeasurementCount = 5;
RTC_DATA_ATTR int overflowCount = 0;

RTC_DATA_ATTR float targetHeightFactor = 0.6f;

// Werte ueber oder unter festgelegte Grenzen

int RefHighCountAmount = 5;
RTC_DATA_ATTR int RefHighCount = 0;

int RefLowCountAmount = 5;
RTC_DATA_ATTR int RefLowCount = 0;

int DownHighCountAmount = 5;
RTC_DATA_ATTR int DownHighCount = 0;

int DownLowCountAmount = 5;
RTC_DATA_ATTR int DownLowCount = 0;

int validationWindow = 5;

float downPlusMargin = 100.0f; // in lux

float minRangeDown = 100.0f, maxRangeDown = 65000.0f, minRangeRef = 100.0f, maxRangeRef = 65000.0f;

int amtMessungen = 60;
int amtIgnore = 5;
float luxArray[60];

void updateLastInOscArray(boolean value){
  int i;
  for (i=0; i<maehenArrLength-1; i++){
    maehenArr[i] = maehenArr[i + 1];
  }
  maehenArr[maehenArrLength-1] = value;
}

boolean decideOscArray(){
  int count = 0;
  int i;
  for (i=0; i<maehenArrLength; i++){
    if (maehenArr[i] == true){
      count++;
    }
  }
  
  if (count >= validationWindow){
    return true;
  }else{
    return false;
  }
}

// Auswertung von maehenArr nur bei effektiver Messung

int getMeasurementCode(int luxRef, int luxDown){
  int code = 0;
  String message = "";
  
  //case 1
  if (luxRef > maxRangeRef){
    code = SENSOR_REF_MALF_HELL;
    message = getMessage(SENSOR_REF_MALF_HELL);
  }else if ((luxRef <= maxRangeRef) && (luxRef >= minRangeRef)){
    //case 3
    if (luxDown > maxRangeDown){
      code = SENSOR_DOWN_MALF_HELL;
      message = getMessage(SENSOR_DOWN_MALF_HELL);
    //case 4
    }else if ((luxDown <= maxRangeDown) && (luxDown >= minRangeDown)){

      if (luxDown < luxRef){
        float heightFactor = 1.0f - (luxDown/luxRef);
        float heightFactorDifference = lastHeightFactor - heightFactor;
        remainingOverflow -= heightFactorDifference;
        remainingOverflow -= expectedMaxStep;
        
        if (remainingOverflow < 0){
          remainingOverflow = 0;
          overflowCount = 0;
          boolean target_reached = false;
          if (heightFactor >= targetHeightFactor){
            updateLastInOscArray(true);
            if (decideOscArray()){
              target_reached = true;
              code = TARGET_REACHED;
              message = getMessage(TARGET_REACHED);
            }
          }else{
            updateLastInOscArray(false);
          }
          if (!target_reached){
            //message = percentage of height // was könnte der Benutzer sehen?
            // return sth !
            code = NORMAL_MEASUREMENT;
          }
        }else{
          overflowCount++;
          if (overflowMeasurementCount >= overflowCount){
            code = SENSOR_DOWN_OBSTACLE;
            message = getMessage(SENSOR_DOWN_OBSTACLE);
          }else{
            code = NORMAL_MEASUREMENT; // ist das richtig?
          }
        }
        
        lastHeightFactor = heightFactor;
      }else{
        if (luxDown <= luxRef + downPlusMargin){
          code = EVEN_LEVEL;
          message = getMessage(EVEN_LEVEL);
        }else{
          code = SENSOR_REF_OBSTACLE;
          message = getMessage(SENSOR_REF_OBSTACLE);
        }
      }
      
    //case 5
    }else{
      if (luxDown < 0.0f){
        code = SENSOR_DOWN_ERROR;
        message = getMessage(SENSOR_DOWN_ERROR);
      }else{
        code = SENSOR_DOWN_MALF_DUNKEL;
        message = getMessage(SENSOR_DOWN_MALF_DUNKEL);
      }
    }
  }else{
  //case 2
    if (luxRef < 0.0f){
      code = SENSOR_REF_ERROR;
      message = getMessage(SENSOR_REF_ERROR);
    }else{
      code = SENSOR_REF_MALF_DUNKEL;
      message = getMessage(SENSOR_REF_MALF_DUNKEL);
    }
  }

  //return message;
  return code;
}

float messenLux(BH1750 sensor) {
    int counter = 0;
    while (counter < amtIgnore) {
        sensor.readLightLevel();
        counter++;
    }

    counter = 0;
    float accMess = 0;
    while (counter < amtMessungen) {
        float lux = sensor.readLightLevel();
        accMess += lux;
        counter++;
    }

    return (accMess / amtMessungen) - 2.0f;
}


bool inRange(float value, float maxLimit, float minLimit) {
    return (value >= minLimit && value <= maxLimit);
}
