#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

//================
#include<Wire.h>

#include <NewPing.h>
#define TRIGGER_PIN  2  
#define ECHO_PIN     4  
#define MAX_DISTANCE 100 

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
int jarak = 0; 
int distance; 
const int Buzzer = 27;  
const int saklar2 = 25;  
const int ldkanan = 22;
const int ldkiri = 26;
const int lskanan = 14;
const int lskiri = 32;

String modeSein = "";

String statusHazard = "false";
String statusSeinKiri = "false";
String statusSeinKanan = "false";
String statusLampuDepan = "false";

int BuzzerState = LOW; 
unsigned long previousMillis = 0;
const long interval = 100; 
const long interval2 = 500;
//==================================================================================================
//==================================================================================================

#include <Servo.h>
Servo myservo, servoku; 
//int pos = 0;
int putar = 0;

const int ServoAngleStep        =   2;      // The angle by which the servo position is changed every step (degree) 
const int ServoAngleMinimum     =   0;      // The min angle of the servo (degrees)
const int ServoAngleMaximum     = 180;

bool stateServo = false;

AsyncWebServer server(80);
//const char *ssid = "kplatolato";
//const char *password = "12345678";
const char *ssid = "@PTMI";
const char *password = "mitrainformatika@123";
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "application/json", "{\"message\":\"Not found\"}");
}
void setup()
{
  Serial.begin(115200);
  myservo.attach(13);
  servoku.attach(12);
  pinMode(Buzzer, OUTPUT); 
  pinMode(saklar2, OUTPUT);
  
   pinMode(ldkanan, OUTPUT);
    pinMode(ldkiri, OUTPUT);
     pinMode(lskanan, OUTPUT);
      pinMode(lskiri, OUTPUT);
     
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
//
//  
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  //=================================================================================

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"message\":\"Welcome\"}");
  });



  //==========================================================================================
   server.on("/jarak", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<100> data;
    
//    if distance = (sonar.ping_cm()); 
//    if(request->hasParam("status")) {
//      AsyncWebParameter* params = request->getParam("status");
//
//      data["status"] = true;
//      data["message"] = (params == "true") ? "Menjalankan sensor jarak" : "Mematikan sensor jarak"; 
//    }
    
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);
    
   });

//=========================================================================

   
    server.on("/water", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<100> data;

    if (request->hasParam("status"))
    {
      AsyncWebParameter* params = request->getParam("status");
      if(params->value() == "true") {
        data["status"] = true;
        data["message"] = "sedang menyalakan semprotan";
        digitalWrite(saklar2 , LOW);
      }else{        
        data["status"] = true;
        data["message"] = "off";
        digitalWrite(saklar2 , HIGH);
      }
    }else{
      data["status"] = false;
      data["message"] = "parameter tidak valid";
    }
//   
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);
    
   });



//===================================================================================


     server.on("/lampudepan", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> data;
    //lampudepan?
    if (request->hasParam("mode") && request->hasParam("status")) 
    {
        AsyncWebParameter* paramMode = request->getParam("mode");
        AsyncWebParameter* paramStatus = request->getParam("status");
        if(paramMode->value() == "hazard") {
            if(paramStatus->value() == "true") {
              data["status"] = true;
              data["message"] = "sedang menyalakan lampu hazard";

//const int ldkanan = 22;
//const int ldkiri = 26;
//const int lskanan = 14;
//const int lskiri = 32;


            }else {
              data["status"] = true;
              data["message"] = "lampu hazard mati";
            }

            statusHazard = paramStatus->value();
            modeSein = "hazard";
         }else if (paramMode->value() == "lampudepan"){ 
          if(paramStatus->value() == "true") {       
              data["status"] = true;
              data["message"] = "sedang menyalakan lampu depan";
              digitalWrite(ldkanan , HIGH);
              digitalWrite(ldkiri , HIGH);
              
           
              }else {

              digitalWrite(ldkanan , LOW);
              digitalWrite(ldkiri , LOW);
          
              data["status"] = true;
              data["message"] = "lampu depan mati";
            } 
            statusLampuDepan = paramStatus->value();
         }
      else if (paramMode->value() == "lampuseinkanan"){  
        if(paramStatus->value() == "true") {      
          data["status"] = true;
          data["message"] = "sedang menyalakan lampu sein kanan";              
        }else {
   
//              digitalWrite (lskanan, LOW);
//              digitalWrite(lskiri , LOW);
              data["status"] = true;
              data["message"] = "lampu seinkanan mati";
            }
            
        modeSein = "lampuseinkanan";
        statusSeinKanan = paramStatus->value();
      }
      else if (paramMode->value() == "lampuseinkiri"){  
        if(paramStatus->value() == "true") {       
        data["status"] = true;
        data["message"] = "sedang menyalakan sein kiri";
            
              digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , HIGH);
              delay(300);
       
              digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , LOW);
              delay(250);
             
      }else {

              digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , LOW);
              data["status"] = true;
              data["message"] = "lampu seinkiri mati";
            }
      
      statusSeinKiri = paramStatus->value();
      modeSein = "lampuseinkiri";

      
    }else{
      data["status"] = false;
      data["message"] = "Tidak terdefinisi";
    }
    }else if(request->hasParam("data")) {
        data["status"] = true;
//        data["data"] = "{\"hazard\":\"true\",\"lampu_depan\":true,\"sein\":[\"kanan\": true, \"kiri\": true]}";
        data["data"]["hazard"] = convertToBool(statusHazard);
        data["data"]["sein"]["kanan"] = convertToBool(statusSeinKanan);
        data["data"]["sein"]["kiri"] = convertToBool(statusSeinKiri);
        data["data"]["lampu_depan"] = convertToBool(statusLampuDepan);
    }else{
      data["status"] = false;
      data["message"] = "Parameter tidak tersedia";
    }


    //=====

//   
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);
    
   });



 //=============================================================================

 
  server.on("/wiper", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<100> data;

    data["status"] = true;
    data["message"] = "Berhasil menyalakan wiper";

    for (int pos = 0; pos <= 180; pos +=2) {
      myservo.write(pos); 
      servoku.write(pos);            
      delay(15);

    }
    for (int pos = 180; pos >= 0; pos -=2) { 
      myservo.write(pos);
      servoku.write(pos);            
      delay(15);

    }
    
    String response;
    serializeJson(data, response);
    request->send(200, "application/json", response);


});

 //=============================================================================
 
//  server.addHandler(handler);
  server.onNotFound(notFound);
  server.begin();
}

bool convertToBool(String val) {
  val.toLowerCase();
  if(val == "true") return true;

  return false;
}

void loop()
{
  jarak = sonar.ping_cm();
    Serial.println("Jarak: "+String(jarak));
    
      if (jarak >= 1 && jarak <= 5) {
//        data["status"] = true;
//        data["message"] = "Awas dalam 1 cm ada obstacle";
  
        digitalWrite(Buzzer, HIGH);
        
      }else if (jarak >= 5 && jarak <= 10) {
//        data["status"] = true;
//        data["message"] = "Awas dalam 5 cm ada obstacle";
  
        digitalWrite(Buzzer, LOW);
        digitalWrite(Buzzer, HIGH);
        delay(50);
        digitalWrite(Buzzer, LOW);
        delay(50);

        
      }else if (jarak >= 10 && jarak <= 15) {
//        data["status"] = true;
//        data["message"] = "Awas dalam 10 cm ada obstacle";
  
        digitalWrite(Buzzer, LOW);
        digitalWrite(Buzzer, HIGH);
        delay(200);
        digitalWrite(Buzzer, LOW);
        delay(200);
        
//      }else if (jarak >= 15 && jarak <= 20){
////        data["status"] = true;
////        data["message"] = "Awas dalam 15 cm ada obstacle";
//        
//        digitalWrite(Buzzer, HIGH);
//        delay(250);
//        digitalWrite(Buzzer, LOW);
//        delay(250);
//        
//      }else if (jarak >= 20 && jarak <= 30){
////        data["status"] = true;
////        data["message"] = "Awas dalam 20 cm ada obstacle";
//        
//        digitalWrite(Buzzer, HIGH);
//        delay(500);
//        digitalWrite(Buzzer, LOW);
//        delay(500);
//        
//      }else if (jarak >= 30 && jarak <= 40){
////        data["status"] = true;
////        data["message"] = "Awas dalam 30 cm ada obstacle";
//        
//        digitalWrite(Buzzer, HIGH);
//        delay(750);
//        digitalWrite(Buzzer, LOW);
//        delay(750);
       
//      }else{
////        data["status"] = false;
////        data["message"] = "AMAN";
//        digitalWrite(Buzzer, LOW);
 }
  if(modeSein == "hazard") {
    if(statusHazard == "true") {
        statusSeinKanan = "false" ;
        statusSeinKiri = "false" ;
      digitalWrite (lskanan, HIGH);
      digitalWrite(lskiri , HIGH);
      delay(300);
      digitalWrite (lskanan, LOW);
      digitalWrite(lskiri , LOW);
      delay(250);
      
    }else{
      digitalWrite(lskanan, LOW);
      digitalWrite(lskiri, LOW);
    }
    
  }
  if(modeSein == "lampuseinkanan") {
    if(statusSeinKanan == "true") {
      statusSeinKiri = "false" ;
      statusHazard = "false" ;
      digitalWrite (lskanan, HIGH);
              digitalWrite(lskiri , LOW);
              delay(300);
           
              digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , LOW);
              delay(250);
    }else{
      digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , LOW);
    }
  }
  if(modeSein == "lampuseinkiri") {
    if(statusSeinKiri == "true") { 
      statusSeinKanan = "false" ;
      statusHazard = "false" ;
      digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , HIGH);
              delay(300);
           
              digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , LOW);
              delay(250);
    }else{
      digitalWrite (lskanan, LOW);
              digitalWrite(lskiri , LOW);
    }
  }
}
