

const int SENSOR_DOWN_OBSTACLE = 1;
const int SENSOR_REF_OBSTACLE = 2;
const int SENSOR_DOWN_MALF_HELL = 3;
const int SENSOR_REF_MALF_HELL = 4;
const int SENSOR_DOWN_MALF_DUNKEL = 5;
const int SENSOR_REF_MALF_DUNKEL = 6;
const int SENSOR_DOWN_ERROR = 7;
const int SENSOR_REF_ERROR = 8;
const int MESSUNG_INFO = 9;
const int TARGET_REACHED = 10;
const int EVEN_LEVEL = 11;
const int NORMAL_MEASUREMENT = 12;

String getMessage(int code){
  if (code == SENSOR_DOWN_OBSTACLE){
    return "Unterer Sensor ist von einem Hindernis bedeckt. Bitte Messtation überptüfen.";
  }
  if (code == SENSOR_REF_OBSTACLE){
    return "Oberer Sensor ist von einem Hindernis bedeckt. Bitte Messtation überptüfen.";
  }
  if (code == SENSOR_DOWN_MALF_HELL){
    return "Unterer Sensor ist zu hell. Bitte Sensor überprüfen.";
  }
  if (code == SENSOR_REF_MALF_HELL){
    return "Oberer Sensor ist zu hell. Bitte Sensor überprüfen.";
  }
  if (code == SENSOR_DOWN_MALF_DUNKEL){
    return "Unterer Sensor ist zu dunkel. Bitte Sensor überprüfen.";
  }
  if (code == SENSOR_REF_MALF_DUNKEL){
    return "Oberer Sensor ist zu dunkel. Bitte Sensor überprüfen.";
  }
  if (code == SENSOR_DOWN_ERROR){
    return "Unterer Sensor liefert Fehlmessungen.\nMöglicherweise ist der Sensor nicht mehr verbunden.\nBitte Kontakte prüfen.";
  }
  if (code == SENSOR_REF_ERROR){
    return "Oberer Sensor liefert Fehlmessungen.\nMöglicherweise ist der Sensor nicht mehr verbunden.\nBitte Kontakte prüfen.";
  }
  if (code == TARGET_REACHED){
    return "Hoehe erreicht! Fläche kann gemaeht werden.";
  }
  if (code == EVEN_LEVEL){
    return "Flaeche wurde gerade gemaeht oder ist noch sehr niedrig.";
  }
  if (code == NORMAL_MEASUREMENT){
    return "Messung: Hoehe auf x Prozent";
  }
}
