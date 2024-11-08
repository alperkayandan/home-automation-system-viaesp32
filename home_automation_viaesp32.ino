#define Pin_RGBR 6 // led kırmızı renk pini
#define Pin_RGBG 7 // led yeşil renk pini
#define Pin_RGBB 8 // led mavi renk pini

#include <math.h>
#if defined(ESP32)
#include <WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Slytherin"
#define WIFI_PASSWORD "Ezgi32913291" 

#define API_KEY "AIzaSyCwaFUj2zh4RAq_6rwMHFBv4A8t0kmt0n0" 
#define DATABASE_URL "sicaklik-degisimi-default-rtdb.firebaseio.com"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

double Termistor(int analogOkuma) {
  double sicaklik;
  sicaklik = log(((10240000 / analogOkuma) - 10000));
  sicaklik = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * sicaklik * sicaklik)) * sicaklik);
  sicaklik = sicaklik - 273.15;
  return sicaklik;
}

void setup(){ 
  Serial.begin(115200);
 
  pinMode(Pin_RGBR,OUTPUT);
  pinMode(Pin_RGBG,OUTPUT);
  pinMode(Pin_RGBB,OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}


void loop(){
  int deger = analogRead(0);
  double sicaklik = Termistor(deger);
  Serial.println(sicaklik);
  
  if (sicaklik > 30) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }

  if(sicaklik >= 10 && sicaklik <=25 ){
         analogWrite(Pin_RGBR ,255);
         analogWrite(Pin_RGBG ,255);
         analogWrite(Pin_RGBB ,0);
         ledRenk = "Mavi";
        }
        else if(sicaklik > 25 && sicaklik <= 40){
         analogWrite(Pin_RGBR ,120);
         analogWrite(Pin_RGBG ,0);
         analogWrite(Pin_RGBB ,255);
         ledRenk = "Sarı";
        }
        else if(sicaklik > 40 && sicaklik < 55){
         analogWrite(Pin_RGBR ,0);
         analogWrite(Pin_RGBG ,100);
         analogWrite(Pin_RGBB ,255);
         ledRenk = "Turuncu";
        }         
        else if(sicaklik >= 55){
         analogWrite(Pin_RGBR ,0);
         analogWrite(Pin_RGBG ,255);
         analogWrite(Pin_RGBB ,255);
         ledRenk = "Kırmızı";
        }
if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 250 || sendDataPrevMillis == 0)){
     sendDataPrevMillis = millis();
      if (Firebase.RTDB.setInt(&fbdo, "temperature/int", sicaklik)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
 }
}
