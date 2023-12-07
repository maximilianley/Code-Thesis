#include <HTTPClient.h>

const String URL = "https://getpantry.cloud/apiv1/pantry/76702abf-cc5d-4084-a611-be8ec68ee3af/basket";
HTTPClient http;

boolean postJSON(String endpoint, String content){
  http.begin(URL+endpoint);
  http.addHeader("Content-Type", "application/json");

  int maxAttempts = 4;
  int attempts = 0;
  boolean success = false;
  int httpResponseCode;
  
  while ((attempts < maxAttempts) && !success){
    String post_content = "{\"nachricht\": \""+content+"\"}";
    httpResponseCode = http.POST(post_content);
    if (httpResponseCode>0){
      success = true;
    }
    attempts++;
  }

  if(success){
  
    //String response = http.getString();
    Serial.println("http Post Success");
    Serial.printf("Response code: %d \n", httpResponseCode);
    return true;
  
  }else{
  
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
    return false;
  
   }
  
   http.end(); 
}
