#include <Arduino.h>

#include <WiFi.h>

#include <ESPAsyncWebServer.h>

#include <AsyncJson.h>

#include <ArduinoJson.h>
 //========================================================================
 //int myPins[] = {2, 4, 8, 3, 6};
int sensorMalfunctionLamp[] = {
    0, //sensor lampu
    0, //sensor sein
    0, //sensor senja
    0, //stoplamp
};
#define pinSensorMalfunctionLamp 0 //sensor malfunction in Lamp
#define pinSensorMalfunctionSein 0 //sensor malfunction in Lamp
#define pinSensorMalfunctionSenja 0 //sensor malfunction in Lamp
#define pinSensorMalfunctionStoplamp 34 //sensor malfunction in Lamp

int isSensorLoopActive = 0;
int isMalfunctionSeinActive = 0;
int isMalfunctionInSein = 0;
int isMalfunctionStop = 0;
int isMalfunctionStopActive = 0;

//for parola ini 
#include <MD_Parola.h>

#include <MD_MAX72xx.h>

#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 14
#define DATA_PIN 13
#define CS_PIN 15

//#define MAX_DEVICES 4
//#define CLK_PIN 14
//#define DATA_PIN 15
//#define CS_PIN 13
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

//konfig lampu
//==========================================================================
String modeSein = "";

String statusHazard = "false";
String statusSeinKiri = "false";
String statusSeinKanan = "false";
String statusLampuDepan = "false";

String statusLampuMerah1 = "false";
String statusLampuPutih1 = "false";

String headShortLampActive = "false";
String headLongLampActive = "false";

//MAX variables kanan, kiri, mid
char ledChar[400];
int kecepatan = 100;
String ledText = "";
bool isMatrixActive = false;
String matrixPosition = "";
textEffect_t scrollEffect;

//==========================================================================

#include<Wire.h>

#include <NewPing.h>

#define TRIGGER_PIN 2
#define ECHO_PIN 4
#define MAX_DISTANCE 100

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
int jarak = 0;
int distance;

//==========================================================================

#include <Servo.h>

Servo myservo, servoku;
int pos = 0;
int putar = 0;

const int ServoAngleStep = 2;
const int ServoAngleMinimum = 0;
const int ServoAngleMaximum = 180;

bool stateCleanLamp = false;
int loopCleanLamp = 0;
unsigned long timeCleanLamp1 = 0;
unsigned long timeCleanLamp2 = 0;
int posServoCleanLamp = 0;

//onserver 

// 
//  server.on("/wiper", HTTP_GET, [](AsyncWebServerRequest *request) {
//    StaticJsonDocument<100> data;
//
//    data["status"] = true;
//    data["message"] = "Berhasil menyalakan wiper";
//
//    for (int pos = 0; pos <= 180; pos +=2) {
//      myservo.write(pos); 
//      servoku.write(pos);            
//      delay(15);
//
//    }
//    for (int pos = 180; pos >= 0; pos -=2) { 
//      myservo.write(pos);
//      servoku.write(pos);            
//      delay(15);
//
//    }
//    
//    String response;
//    serializeJson(data, response);
//    request->send(200, "application/json", response);
//
//
//});

//==================================================================================

//blink 
bool ledStatehazard = LOW;
bool ledStateseinkanan = LOW;
bool ledStateseinkiri = LOW;

unsigned long waktuhazard = 0;
unsigned long waktuseinkiri = 0;
unsigned long waktuseinkanan = 0;

const int ldkanan = 22; //stoplamp = merah
const int ldkiri = 26; //senjalamp = putih
const int lskanan = 14;
const int lskiri = 32;

const int Buzzer = 27;
const int lampujauh = 25;
const int lampudekat = 33;

//MAX7219
#define BUF_SIZE 75
char curMessage[BUF_SIZE] = {
   ""
};
char newMessage[BUF_SIZE] = {
   "STARTING"
};
bool newMessageAvailable = false;

String posisiDepan = "A";

//=============================================================
//Head

//VARIABLES PWM
#define delayDuty 1

bool senjaLampIdleModeRun = false;
bool senjaLampStartModeRun = false;
bool senjaLampOffModeRun = false;

bool stopLampIdleModeRun = false;
bool stopLampStartModeRun = false;
bool stopLampOffModeRun = false;

bool senjaLampIsFromStart = false;
bool stopLampIsFromStart = false;

int dutySenjaLamp = 0;
int dutyStopLamp = 0;

bool isLdPutihActive = false;
bool isLdMerahActive = false;

const int freq = 5000;
const int ledKananChannel = 1;
const int ledKiriChannel = 2;
const int resolution = 8;
//VARIABLES PWM

unsigned long start_timePutih;
unsigned long current_timePutih; // millis() function returns unsigned long

unsigned long start_timeMerah;
unsigned long current_timeMerah; // millis() function returns unsigned long

AsyncWebServer server(80);

//const char *ssid = "kplatolato";
//const char *password = "12345678";

//const char *ssid = "@PTMI";
//const char *password = "mitrainformatika@123";

//const char *ssid = "MITRA INFORMATIKA";
//const char *password = "123456789";

const char * ssid = "LAPTOP-NR7GP59N";
const char * password = "12345678";

//data
bool convertToBool(String val) {
   val.toLowerCase();
   if (val == "true") return true;

   return false;
}

void notFound(AsyncWebServerRequest * request) {
   request->send(404, "application/json", "{\"message\":\"Not found\"}");
}

void setup() {
   Serial.begin(115200);
   //  pinMode(ldkanan, OUTPUT);
   //  pinMode(ldkiri, OUTPUT);
   pinMode(lskanan, OUTPUT);
   pinMode(lskiri, OUTPUT);
   myservo.attach(13);
   servoku.attach(12);
   pinMode(Buzzer, OUTPUT);
   pinMode(lampujauh, OUTPUT);
   pinMode(lampudekat, OUTPUT);

   ledcSetup(ledKananChannel, freq, resolution);
   ledcSetup(ledKiriChannel, freq, resolution);
   ledcAttachPin(ldkanan, ledKananChannel);
   ledcAttachPin(ldkiri, ledKiriChannel);

   current_timePutih = millis();
   start_timePutih = current_timePutih;

   current_timeMerah = millis();
   start_timeMerah = current_timeMerah;

   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);

   //max
   P.begin();
   P.setIntensity(0);

   while(WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.printf("WiFi Failed!\n");
   }

   Serial.print("IP Address: ");
   Serial.println(WiFi.localIP());

   server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
      request->send(200, "application/json", "{\"message\":\"Welcome\"}");
   });

   //=======================================================================================SETTING MODUL
   server.on("/pengaturan", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument<200> data;

      if (request->hasParam("bagian")) {
         //      AsyncWebParameter* paramModul = request->getParam("modul");
         AsyncWebParameter * paramBagian = request->getParam("bagian");

         //      paramModul = request->getParam("modul");
         if (paramBagian->value() == "positionhead") {
            posisiDepan = (posisiDepan != "A") ? "A" : "B";
            data["status"] = true;
            data["front"] = posisiDepan;
            data["back"] = (posisiDepan == "A") ? "B" : "A";
            data["message"] = "Berhasil";

            if (posisiDepan == "A") { //senjalamp
               isLdMerahActive = true;
               stopLampOffModeRun = true;

//               statusLampuMerah1 = "false";

               isLdPutihActive = true;
               if (statusLampuPutih1 == "true") senjaLampStartModeRun = true;
               if (headShortLampActive == "true") {
                  digitalWrite(lampudekat, HIGH);
               }
               if (headLongLampActive == "true") {
                  digitalWrite(lampujauh, HIGH);
               }
            }
            if (posisiDepan == "B") { //stoplamp
               isLdPutihActive = true;
               if (statusLampuPutih1 == "true") senjaLampOffModeRun = true;

               isLdMerahActive = true;
               stopLampIdleModeRun = true;
//   
//   if(sensorMalfunctionLamp[3] == 1) {
//      stopLampIdleModeRun = "true";
//      stopLampIdleModeRun = "true";
//  }else{
//      stopLampOffModeRun = "true";
//      stopLampOffModeRun = "true";
//  }
               

               if (headShortLampActive == "true") {
                  digitalWrite(lampudekat, LOW);
               }
               if (headLongLampActive == "true") {
                  digitalWrite(lampujauh, LOW);
               }

    
   

//               statusLampuMerah1 = "true";
            }
         } else {
            data["status"] = false;
            data["message"] = "Gagal setting, karena parameter salah";
         }
      } else if (request->hasParam("data")) {
         data["status"] = true;

         //LAMP
         data["modules"]["front"] = posisiDepan;
         data["modules"]["back"] = (posisiDepan == "A") ? "B" : "A";

         //MATRIX
         data["matrix"]["text"] = ledText;
         data["matrix"]["position"] = matrixPosition;
         data["matrix"]["speed"] = kecepatan;
         data["matrix"]["status"] = isMatrixActive;

         data["message"] = "Berhasil";
      } else {
         data["status"] = false;
         data["message"] = "parameter tidak valid";
      }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);
   });

   //=======================================================================================SETTING SERVO
   server.on("/wiper", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument<100> data;
      Serial.println("servo");

      data["status"] = true;
      data["message"] = "Berhasil menyalakan wiper";

      //    stateCleanLamp = true;
      //    loopCleanLamp = 0;
      //
      //    for (int pos = 0; pos <= 180; pos +=2) {
      //      myservo.write(pos); 
      //      servoku.write(pos);            
      //      delay(15);
      //    }
      //    for (int pos = 180; pos >= 0; pos -=2) { 
      //      myservo.write(pos);
      //      servoku.write(pos);            
      //      delay(15);
      //    }

      //      if (request->hasParam("servo"))
      //        AsyncWebParameter* paramServo = request->getParam("servo");
      //    { 
      //         for (int pos = 0; pos <= 90; pos +=2) 
      //       {
      //           myservo.write(pos); 
      //           servoku.write(pos);            
      //           delay(15);
      //
      //       }
      //    }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);
   });

   //=======================================================================================SETTING LAMPU 
   server.on("/lampu", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument<200> data;
      //lampudepan?
      if (request->hasParam("mode") && request->hasParam("status")) {
         AsyncWebParameter * paramMode = request->getParam("mode");
         AsyncWebParameter * paramStatus = request->getParam("status");

         if (paramMode->value() == "hazard") { //hazard
            if (paramStatus->value() == "true") {
               data["status"] = true;
               data["message"] = "sedang menyalakan lampu hazard";
//               isMalfunctionSeinActive = 1;
               if (statusLampuPutih1 == "true" && posisiDepan == "A") senjaLampIdleModeRun = true;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampIdleModeRun = true;
            } else {
               data["status"] = true;
               data["message"] = "lampu hazard mati";
//               isMalfunctionSeinActive = 0;
               if (statusLampuPutih1 == "true" && posisiDepan == "A") senjaLampStartModeRun = true;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampStartModeRun = true;
            }

            statusHazard = paramStatus->value();
            modeSein = "hazard";
         } else if (paramMode->value() == "lampu") { //lampudepannya
            //        if (paramBagian->value()=="A") {
            if (posisiDepan == "A") { //MODUL A 
               isLdPutihActive = true;
               
               if (paramStatus->value() == "true") {
                  //              digitalWrite(ldkiri, HIGH);
                  //              digitalWrite(ldkanan, LOW);
                  
                  if (statusHazard == "true" || statusSeinKiri == "true" || statusSeinKanan == "true") senjaLampIdleModeRun = true;
                  else senjaLampStartModeRun = true;
                  //                isFromStart = true;

                  data["status"] = true;
                  data["message"] = "sedang menyalakan lampu senja";
               } else {
                  //              digitalWrite(ldkiri, LOW);
                  senjaLampOffModeRun = true;
       
                  data["status"] = true;
                  data["message"] = "lampu A mati";
               }

               statusLampuPutih1 = paramStatus->value();
               //            modeSein = "A";
            }

            //          if (paramBagian->value()=="B")
            if (posisiDepan == "B") { //MODUL B
               isLdMerahActive = true;
               if (paramStatus->value() == "true") {

                  isMalfunctionStopActive = 1;
                  stopLampStartModeRun = true;
                  //              if(statusHazard == "true" || statusSeinKiri == "true" || statusSeinKanan == "true") {
                  //                if(statusLampuMerah1 == "true") stopLampIdleModeRun = true;
                  //              }else {
                  //                  if(statusLampuMerah1 == "true") stopLampStartModeRun = true;
                  //                    else stopLampIdleModeRun = true;
                  //                }

                  digitalWrite(ldkanan, HIGH);
                  digitalWrite(ldkiri, LOW);
               
                  data["status"] = true;
                  data["message"] = "sedang menyalakan lampu senja";
               } else {
                 
                  //                    digitalWrite(ldkanan, LOW);
                  stopLampIdleModeRun = true;
                  isMalfunctionStopActive = 0;
  

                  data["status"] = true;
                  data["message"] = "lampu B mati";
               }
               statusLampuMerah1 = paramStatus->value();
            }
            //SENDDDD
                     } else if (paramMode->value() == "dekat") {
            if (posisiDepan == "A") {
               headShortLampActive = paramStatus->value();
               if (paramStatus->value() == "true") {
//                  sensorMalfunctionLamp = digitalRead(pinSensorMalfunctionLamp);
//                  headShortLampActive = paramStatus->value();
                  // headLongLampActive = paramStatus->value();
                  digitalWrite(lampudekat, HIGH);
                  isSensorLoopActive = 1;
//                  delay(2000);
                  
//                  if(sensorMalfunctionLamp != 1 && headShortLampActive == "true") {
////                    Serial.println("malfunction");
//                    digitalWrite(lampudekat, LOW);
//                    digitalWrite (lampujauh, LOW);
//                    headShortLampActive = "false";
//
//                    if(isSensorLoopActive == 1) {
//                      isSensorLoopActive = 0;
//                      Serial.println("isSensorLoopActive false");
//                    }
//                    
//                    Serial.print("sensor: ");
//                    Serial.print(sensorMalfunctionLamp);
//                    Serial.print(" ");
//                    Serial.print("lampShort: ");
//                    Serial.println(headShortLampActive);
//                    
//                    data["status"] = false;
//                    data["message"] = "Gagal menghidupkan lampu dekat, dikarenakan terjadi kesalahan pada hardware";
//                  }else if(sensorMalfunctionLamp == 1 &&  headShortLampActive == "true") {
////                    Serial.println("good");
//
//                    //digitalWrite(lampudekat, HIGH);
//                    // digitalWrite (lampujauh, LOW);
                    
                    data["status"] = true;
                    data["message"] = "Proses menyalakan Lampu Dekat";
//                  }else{
//                    digitalWrite(lampudekat, LOW);
//                    digitalWrite (lampujauh, LOW);
//                    headShortLampActive = "false";
//
//                    if(isSensorLoopActive == 1) {
//                      isSensorLoopActive = 0;
//                      Serial.println("isSensorLoopActive false");
//                    }
                    
//                    Serial.print("BAD: ");
//                    Serial.print("sensor: ");
//                    Serial.print(sensorMalfunctionLamp);
//                    Serial.print(" ");
//                    Serial.print("lampShort: ");
//                    Serial.println(headShortLampActive);

//                    data["status"] = false;
//                    data["message"] = "cannot read response module sensor, please check hardware.";
//                  }
               }
               if (paramStatus->value() == "false") {
                  digitalWrite(lampudekat, LOW);
                  digitalWrite(lampujauh, LOW);
                  isSensorLoopActive = 0;

                  data["status"] = true;
                  data["message"] = "Lampu Dekat Mati";

                  headLongLampActive = paramStatus->value();
               }
            }
            //            if (posisiDepan=="B"){
            //              digitalWrite (lampudekat, LOW);
            //              digitalWrite (lampujauh, LOW);
            //  
            //              data["status"] = true;
            //              data["message"] = "Lampu Dekat Mati";
            //            }
         } else if (paramMode->value() == "jauh") {
            if (posisiDepan == "A") {
//               sensorMalfunctionLamp = digitalRead(pinSensorMalfunctionLamp);
//               headShortLampActive = paramStatus->value();
               headLongLampActive = paramStatus->value();
               if (paramStatus->value() == "true") {
                    digitalWrite(lampudekat, HIGH);
                    digitalWrite(lampujauh, HIGH);
                    isSensorLoopActive = 1;

                    data["status"] = true;
                    data["message"] = "Proses menyalakan Lampu Jauh";
//                  if(sensorMalfunctionLamp != 1 &&  (headShortLampActive == "true" && headLongLampActive == "true")) {
////                    Serial.println("malfunction");
//
////                    digitalWrite(lampudekat, HIGH);
//                    digitalWrite(lampujauh, LOW);
//                    
//                    data["status"] = false;
//                    data["message"] = "Gagal menghidupkan lampu, dikarenakan terjadi kesalahan pada hardware";
//                  }else if(sensorMalfunctionLamp == 1 &&  (headShortLampActive == "true" && headLongLampActive == "true")) {
////                    Serial.println("good");
//                    digitalWrite(lampudekat, HIGH);
//                    digitalWrite(lampujauh, HIGH);
//                    
//                    data["status"] = true;
//                    data["message"] = "Lampu Jauh Menyala";
//                  }else{
//                    digitalWrite(lampujauh, LOW);
//                    
//                    Serial.print("BAD: ");
//                    Serial.print("sensor: ");
//                    Serial.print(sensorMalfunctionLamp);
//                    Serial.print(" ");
//                    Serial.print("lampShort: ");
//                    Serial.print(headShortLampActive);
//                    Serial.print("lampLong: ");
//                    Serial.println(headLongLampActive);
//
//                    data["status"] = false;
//                    data["message"] = "cannot read response module sensor, please check hardware.";
//                  }
               }
               if (paramStatus->value() == "false") {
//                  digitalWrite(lampudekat, LOW);
                  digitalWrite(lampujauh, LOW);
                  isSensorLoopActive = 0;

                  data["status"] = true;
                  data["message"] = "Lampu Jauh Mati";
               }
            }

//         } else if (paramMode->value() == "dekat") {
//            if (posisiDepan == "A") {
//               if (paramStatus->value() == "true") {
//                  digitalWrite(lampudekat, HIGH);
//                  //                digitalWrite (lampujauh, LOW);
//
//                  data["status"] = true;
//                  data["message"] = "Lampu Dekat Menyala";
//               }
//               if (paramStatus->value() == "false") {
//                  digitalWrite(lampudekat, LOW);
//                  digitalWrite(lampujauh, LOW);
//
//                  data["status"] = true;
//                  data["message"] = "Lampu Dekat Mati";
//
//                  headLongLampActive = paramStatus->value();
//               }
//
//               headShortLampActive = paramStatus->value();
//               //              headLongLampActive = paramStatus->value();
//            }
//            //            if (posisiDepan=="B"){
//            //              digitalWrite (lampudekat, LOW);
//            //              digitalWrite (lampujauh, LOW);
//            //  
//            //              data["status"] = true;
//            //              data["message"] = "Lampu Dekat Mati";
//            //            }
//         } else if (paramMode->value() == "jauh") {
//            if (posisiDepan == "A") {
//               if (paramStatus->value() == "true") {
////                  digitalWrite(lampudekat, HIGH);
//                  digitalWrite(lampujauh, HIGH);
//
//                  data["status"] = true;
//                  data["message"] = "Lampu Jauh Menyala";
//               }
//               if (paramStatus->value() == "false") {
////                  digitalWrite(lampudekat, LOW);
//                  digitalWrite(lampujauh, LOW);
//
//                  data["status"] = true;
//                  data["message"] = "Lampu Jauh Mati";
//               }
//
////               headShortLampActive = paramStatus->value();
//               headLongLampActive = paramStatus->value();
//            }
         } else if (paramMode->value() == "lampuseinkanan") { //SEIN KANAN = Next dengan FLIP FLOP 
            if (paramStatus->value() == "true") {
               data["status"] = true;
               data["message"] = "sedang menyalakan lampu sein kanan";
//                isMalfunctionSeinActive = 1;
               if (statusLampuPutih1 == "true" && posisiDepan == "A") senjaLampIdleModeRun = true;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampIdleModeRun = true;                
            } else {
               data["status"] = true;
               data["message"] = "lampu seinkanan mati";
//                isMalfunctionSeinActive = 0;
               if (statusLampuPutih1 == "true" && posisiDepan == "A") senjaLampStartModeRun = true;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampStartModeRun = true;
            }

            modeSein = "lampuseinkanan";
            statusSeinKanan = paramStatus->value();
         } else if (paramMode->value() == "lampuseinkiri") { //SEIN KIRI = Next dengan FLIP FLOP   
            if (paramStatus->value() == "true") {
               data["status"] = true;
               data["message"] = "sedang menyalakan sein kiri";
                isMalfunctionSeinActive = 1;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampIdleModeRun = true;
               if (statusLampuPutih1 == "true" && posisiDepan == "A") senjaLampIdleModeRun = true;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampIdleModeRun = true;
            } else {
               data["status"] = true;
               data["message"] = "lampu seinkiri mati";
                isMalfunctionSeinActive = 0;
               if (statusLampuPutih1 == "true" && posisiDepan == "A") senjaLampStartModeRun = true;
               //          if(statusLampuMerah1 == "true" && posisiDepan == "B") stopLampStartModeRun = true;
            }

            statusSeinKiri = paramStatus->value();
            modeSein = "lampuseinkiri";
         } else {
            data["status"] = false;
            data["message"] = "Tidak terdefinisi";
         }
      } else if (request->hasParam("data")) {
         data["status"] = true;
//         data["data"] = "{\"hazard\":\"true\",\"lampu_depan\":true,\"sein\":[\"kanan\": true, \"kiri\": true]}";
         data["data"]["hazard"] = convertToBool(statusHazard);
         data["data"]["sein"]["kanan"] = convertToBool(statusSeinKanan);
         data["data"]["sein"]["kiri"] = convertToBool(statusSeinKiri);
         data["data"]["lampudepan"]["senja"] = convertToBool(statusLampuPutih1);
         data["data"]["lampudepan"]["dekat"] = convertToBool(headShortLampActive);
         data["data"]["lampudepan"]["jauh"] = convertToBool(headLongLampActive);
         data["data"]["stoplamp"] = convertToBool(statusLampuMerah1);

          data["data"]["malfunction"]["lampu"] = (sensorMalfunctionLamp[0] == 1) ? false : ((sensorMalfunctionLamp[0] == 0 && (headShortLampActive == "true" || headLongLampActive == "true")) ? true : false);
         data["data"]["malfunction"]["sein"] = (isMalfunctionInSein == 0 ? false : true);
         data["data"]["malfunction"]["senja"] = (sensorMalfunctionLamp[2] == 1) ? false : ((sensorMalfunctionLamp[2] == 0 && headShortLampActive == "true") ? true : false);
         data["data"]["malfunction"]["stoplamp"] = (sensorMalfunctionLamp[3] == 1) ? false : ((sensorMalfunctionLamp[3] == 0 && headShortLampActive == "true") ? true : false);
      } else {
         data["status"] = false;
         data["message"] = "Parameter tidak tersedia";
      }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);

   });

   //=======================================================================================MATRIX
   server.on("/ledkiri", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument<100> data;

      ledText = "";
      int strLen = 0;
      kecepatan = 0;
      char out_str[200]; // output from string functions placed here
      int num; // general purpose integer

      if (request->hasParam("text") && request->hasParam("kecepatan")) {
         //      AsyncWebParameter* txtParam = request->getParam("text");
         ledText = request->getParam("text")->value();
         kecepatan = String(request->getParam("kecepatan")->value()).toInt();
         //      num = strlen(ledText);

         //      strcpy(out_str, ledText);

         data["status"] = true;
         data["message"] = "Berhasil menjalankan led ke kiri: " + ledText;
         data["speed"] = kecepatan;

         newMessageAvailable = true;
         isMatrixActive = true;
         matrixPosition = "ledkiri";

         Serial.print("Kecepatan: ");
         Serial.println(String(request->getParam("kecepatan")->value()));
      } else {
         data["status"] = false;
         data["message"] = "Gagal";
      }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);

      scrollEffect = PA_SCROLL_LEFT;

      P.displayClear();
      P.displayText(ledChar, PA_CENTER, kecepatan, 0, scrollEffect, scrollEffect); //text.c_str()
   });

   server.on("/ledmirror", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument<100> data;

      ledText = "";
      int strLen = 0;
      kecepatan = 0;
      char out_str[200]; // output from string functions placed here
      int num; // general purpose integer

      if (request->hasParam("text") && request->hasParam("posisi")) {
         AsyncWebParameter * paramposisi = request->getParam("posisi");
         //      AsyncWebParameter* txtParam = request->getParam("text");

         ledText = request->getParam("text")->value();
         //      num = strlen(ledText);
         //      strcpy(out_str, ledText);

         data["status"] = true;
         data["message"] = "Berhasil menjalankan led ke kiri: " + ledText;

         if (paramposisi->value() == "depan") {
            //        char ledChar[400];
            ledText.toCharArray(ledChar, sizeof(ledChar));

            P.displayClear();
            P.setZoneEffect(0, true, PA_FLIP_LR);
            P.displayText(ledChar, PA_CENTER, 100, 1000, PA_SCROLL_UP, PA_SCROLL_DOWN);
         }
         if (paramposisi->value() == "belakang") {
            //        char ledChar[400];
            ledText.toCharArray(ledChar, sizeof(ledChar));

            P.displayClear();
            P.displayText(ledChar, PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT);

         }
      } else {
         data["status"] = false;
         data["message"] = "Gagal";
      }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);
   });
   //PA_FLIP_UD

   //=======================================================================================
   server.on("/ledkanan", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument<100> data;

      ledText = "";
      int strLen = 0;
      kecepatan = 0;
      char out_str[200]; // output from string functions placed here
      int num; // general purpose integer

      if (request->hasParam("text") && request->hasParam("kecepatan")) {
         //      AsyncWebParameter* txtParam = request->getParam("text");
         ledText = request->getParam("text")->value();
         kecepatan = String(request->getParam("kecepatan")->value()).toInt();
         //      num = strlen(ledText);

         //      strcpy(out_str, ledText);

         data["status"] = true;
         data["message"] = "Berhasil menjalankan led ke kanan: " + ledText;
         data["speed"] = kecepatan;
         newMessageAvailable = true;
         isMatrixActive = true;
         matrixPosition = "ledkanan";

         Serial.print("Kecepatan: ");
         Serial.println(String(request->getParam("kecepatan")->value()));
      } else {
         data["status"] = false;
         data["message"] = "Gagal";
      }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);

      //    char ledChar[300];
      ledText.toCharArray(ledChar, sizeof(ledChar));

      scrollEffect = PA_SCROLL_RIGHT;

      P.displayClear();
      P.displayText(ledChar, PA_CENTER, kecepatan, 0, scrollEffect, scrollEffect); //text.c_str()
   });

   //=======================================================================================
   server.on("/ledmid", HTTP_GET, [](AsyncWebServerRequest * request) {
      StaticJsonDocument < 100 > data;

      ledText = "";
      int strLen = 0;
      kecepatan = 0;
      char out_str[200]; // output from string functions placed here
      int num; // general purpose integer

      if (request->hasParam("text")) {
         ledText = request->getParam("text")->value();

         data["status"] = true;
         data["message"] = "Berhasil menjalankan led ke mid: " + ledText;
         newMessageAvailable = true;
         isMatrixActive = true;
         matrixPosition = "ledmid";

         Serial.print("Kecepatan: ");
         Serial.println(kecepatan);
      } else {
         data["status"] = false;
         data["message"] = "Gagal";
      }

      String response;
      serializeJson(data, response);
      request->send(200, "application/json", response);

      //  char ledChar[300];
      ledText.toCharArray(ledChar, sizeof(ledChar));

      scrollEffect = PA_NO_EFFECT;
      kecepatan = 100;

      P.displayClear();
      P.displayText(ledChar, PA_CENTER, kecepatan, 0, scrollEffect, scrollEffect); //text.c_str()    
   });

   //  server.addHandler(handler);
   server.onNotFound(notFound);
   DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*"); //cros program default
   server.begin();
}

//=====================================================================================cmd 
void runled() {
   if (P.displayAnimate()) {
      P.displayText(ledChar, PA_CENTER, kecepatan, 0, scrollEffect, scrollEffect); //text.c_str()
      P.displayReset();
   }
   if (posisiDepan == "B") {
      P.setZoneEffect(0, true, PA_FLIP_LR);
   }
   if (posisiDepan == "A") {
      P.setZoneEffect(0, false, PA_FLIP_LR);
   }
}

void ultras() {
   jarak = sonar.ping_cm();
   //  Serial.println("Jarak: "+String(jarak));

   if (jarak >= 1 && jarak <= 5) {
      //    data["status"] = true;
      //    data["message"] = "Awas dalam 1 cm ada obstacle";

      digitalWrite(Buzzer, HIGH);

   } else if (jarak >= 5 && jarak <= 10) {
      //    data["status"] = true;
      //    data["message"] = "Awas dalam 5 cm ada obstacle";

      digitalWrite(Buzzer, LOW);
      digitalWrite(Buzzer, HIGH);
      delay(50);
      digitalWrite(Buzzer, LOW);
      delay(50);
   } else if (jarak >= 10 && jarak <= 15) {
      //    data["status"] = true;
      //    data["message"] = "Awas dalam 10 cm ada obstacle";

      digitalWrite(Buzzer, LOW);
      digitalWrite(Buzzer, HIGH);
      delay(200);
      digitalWrite(Buzzer, LOW);
      delay(200);
   }
}

void seinsistem() {
   if (modeSein == "hazard") {
      if (statusHazard == "true") {
         statusSeinKanan = "false";
         statusSeinKiri = "false";

         unsigned long waktusekarang = millis();
         if ((waktusekarang - waktuhazard) >= 500) {
            ledStatehazard = !ledStatehazard;
            digitalWrite(lskanan, ledStatehazard);
            digitalWrite(lskiri, ledStatehazard);
            waktuhazard = millis();
         }
      } else {
         digitalWrite(lskanan, LOW);
         digitalWrite(lskiri, LOW);
      }
   }

   if (modeSein == "lampuseinkanan") {
      if (statusSeinKanan == "true") {
         statusSeinKiri = "false";
         statusHazard = "false";
         digitalWrite(lskiri, LOW);
         unsigned long waktusekarang = millis();
         if ((waktusekarang - waktuseinkanan) >= 500) {
            ledStateseinkanan = !ledStateseinkanan;
            digitalWrite(lskanan, ledStateseinkanan);
            waktuseinkanan = millis();
         }
      } else {
         digitalWrite(lskanan, LOW);
         digitalWrite(lskiri, LOW);
      }
   }

   if (modeSein == "lampuseinkiri") {
      if (statusSeinKiri == "true") {
         statusSeinKanan = "false";
         statusHazard = "false";
         digitalWrite(lskanan, LOW);
         unsigned long waktusekarang = millis();
         if ((waktusekarang - waktuseinkiri) >= 500) {
            ledStateseinkiri = !ledStateseinkiri;
            digitalWrite(lskiri, ledStateseinkiri);
            waktuseinkiri = millis();
         }
      } else {
         digitalWrite(lskanan, LOW);
         digitalWrite(lskiri, LOW);
      }
   }
   //  if(statusLampuPutih1 == "true") {
   //      if(posisiDepan == "A") {
   //        isLdPutihActive = true;
   //  //      isLdMerahActive = false;
   //  //      pwmSetting(4, false, false);
   //  //      digitalWrite(ldkiri, HIGH);
   //  //      digitalWrite(ldkanan, LOW);
   //        
   //      }
   //  //    if(posisiDepan == "B") {
   //  ////      isLdPutihActive = false;
   //  ////      isLdMerahActive = true;
   //  //      //stopLampIdleModeRun = true;
   //  ////      pwmSetting(4, false, true);
   //  ////      digitalWrite(ldkiri, LOW);
   //  ////      digitalWrite(ldkanan, HIGH);
   //  //    } 
   //  }
   //  if(statusLampuPutih1 == "true") {
   //      if(posisiDepan == "B") {
   //  //      isLdPutihActive = false;
   //  //      isLdMerahActive = true;
   //        //stopLampIdleModeRun = true;
   //  //      pwmSetting(4, false, true);
   //  //      digitalWrite(ldkiri, LOW);
   //  //      digitalWrite(ldkanan, HIGH);
   //      } 
   //  }

}

void cleanlamp() {
   //  for (int pos = 0; pos <= 90; pos +=2) {
   //    myservo.write(pos); 
   //    servoku.write(pos);            
   //    delay(15);
   //  }
   if (stateCleanLamp) {
      unsigned long waktusekarang = millis();
      if ((waktusekarang - timeCleanLamp1) >= 20) {
         if (posServoCleanLamp <= 180) {
            pos += 2;
            myservo.write(pos);
            servoku.write(pos);
         }
         timeCleanLamp1 = millis();
      }
      if ((waktusekarang - timeCleanLamp2) >= 20) {
         if (posServoCleanLamp >= 180) {
            pos -= 2;
            myservo.write(pos);
            servoku.write(pos);
         }
         timeCleanLamp2 = millis();
      }
      //    if(loopCleanLamp <= 15) {
      myservo.write(pos);
      servoku.write(pos);
      //    }
      //    if(loopCleanLamp <= 15) {
      //      myservo.write(pos);
      //      servoku.write(pos);
      //    }

      loopCleanLamp++;
   }
}

//===================================================================

//===================================================================
//LAMPU PWM
void idleMode() {
   //  if(isLdPutihActive) {
   if (senjaLampIdleModeRun) {
      if (dutySenjaLamp == 30) {
         dutySenjaLamp = 30;
         isLdPutihActive = false;
         senjaLampIdleModeRun = false;
         senjaLampIsFromStart = false;
      } else {
         pwmLdPutihConf(delayDuty, !(senjaLampIsFromStart));
      }
   }
   //  }
   //  if(isLdMerahActive) {
   if (stopLampIdleModeRun) {
      if (dutyStopLamp == 30) {
         dutyStopLamp = 30;
         isLdMerahActive = false;
         stopLampIdleModeRun = false;
         stopLampIsFromStart = false;
      } else {
         pwmLdMerahConf(delayDuty, !(stopLampIsFromStart));
      }
   }
   //  }
}
void startMode() {
   //  if(isLdPutihActive) {
   if (senjaLampStartModeRun) {
      if (dutySenjaLamp == 250) {
         isLdPutihActive = false;
         senjaLampIsFromStart = true;
         senjaLampStartModeRun = false;
      } else {
         pwmLdPutihConf(4, true);
      }
   }
   //  }
   //  if(isLdMerahActive) {
   if (stopLampStartModeRun) {
      if (dutyStopLamp == 200) {
         isLdMerahActive = false;
         stopLampIsFromStart = true;
         stopLampStartModeRun = false;
      } else {
         pwmLdMerahConf(delayDuty, true);
      }
   }
   //  }
}
void offMode() {
   //  if(isLdPutihActive) {
   if (senjaLampOffModeRun) {
      if (dutySenjaLamp == 0) {
         isLdPutihActive = false;
         senjaLampOffModeRun = false;
      } else {
         //      int delayOffMode = delayDuty;
         //      if(isFromStart) delayOffMode = 2;
         pwmLdPutihConf(4, false);
      }
   }
   //  }
   //  if(isLdMerahActive) {
   if (stopLampOffModeRun) {
      if (dutyStopLamp == 0) {
         isLdMerahActive = false;
         stopLampOffModeRun = false;
      } else {
         //      int delayOffMode = delayDuty;
         //      if(isFromStart) delayOffMode = 2;
         pwmLdMerahConf(delayDuty, false);
      }
   }
   //  }
}

void pwmLdPutihConf(unsigned long timed_event, bool isCountUp) {
   current_timePutih = millis(); // update the timer every cycle

   if (current_timePutih - start_timePutih >= timed_event && isLdPutihActive) {
      if (isCountUp) {
         //      if(isLdPutihActive) {
         if (dutySenjaLamp == 250) dutySenjaLamp = 250;
         else dutySenjaLamp++;

         Serial.print("dutySenjaLamp: ");
         Serial.println(dutySenjaLamp);
         //      }
      } else {
         //      if(isLdPutihActive) {
         if (dutySenjaLamp == 0) dutySenjaLamp = 0;
         else dutySenjaLamp--;

         Serial.print("dutySenjaLamp: ");
         Serial.println(dutySenjaLamp);
         //      }
      }

      start_timePutih = current_timePutih; // iterate duty and reset the timer
      //    if(isLdPutihActive) 
      ledcWrite(ledKiriChannel, dutySenjaLamp);
   }
}

void pwmLdMerahConf(unsigned long timed_event, bool isCountUp) {
   current_timeMerah = millis(); // update the timer every cycle

   if (current_timeMerah - start_timeMerah >= timed_event && isLdMerahActive) {
      if (isCountUp) {
         //      if(isLdMerahActive) {
         if (dutyStopLamp == 200) dutyStopLamp = 200;
         else dutyStopLamp++;

         Serial.print("dutyStopLamp: ");
         Serial.println(dutyStopLamp);
         //      }
      } else {
         //      if(isLdMerahActive) {
         if (dutyStopLamp == 0) dutyStopLamp = 0;
         else dutyStopLamp--;

         Serial.print("dutyStopLamp: ");
         Serial.println(dutyStopLamp);
         //      }
      }

      start_timeMerah = current_timeMerah; // iterate duty and reset the timer
      //    if(isLdMerahActive) 
      ledcWrite(ledKananChannel, dutyStopLamp);
   }
}

void malfunctionlamp() {
//    0, //sensor lampu
//    0, //sensor sein
//    0, //sensor stoplamp
  sensorMalfunctionLamp[0] = digitalRead(pinSensorMalfunctionLamp);
  sensorMalfunctionLamp[1] = digitalRead(pinSensorMalfunctionSein);
  sensorMalfunctionLamp[2] = digitalRead(pinSensorMalfunctionSenja);
  sensorMalfunctionLamp[3] = digitalRead(pinSensorMalfunctionStoplamp);

  if(Serial.available() > 0) isSensorLoopActive = Serial.readString().toInt();
  if(isSensorLoopActive == 1) {
    Serial.print("sensor: ");
    Serial.println(sensorMalfunctionLamp[3]);
  }
  
//   if(isMalfunctionStopActive) {
//      if(sensorMalfunctionLamp[3] == 1 && ( stopLampIdleModeRun = "true")) {
//       
//            Serial.println("malfuvwxyz");
//            isMalfunctionStop = 1;
//            
//          }else{
//            Serial.println("OK else1");
//           
//          }
//          if(sensorMalfunctionLamp[3] == 0 && (stopLampOffModeRun = "true")) {
//            Serial.println("OK");
//            isMalfunctionStop = 0;
//          }
//      }else{
//        Serial.println("OK else2");
//  
//      }

//  if( isMalfunctionStop == 1 &&  stopLampIdleModeRun == "true") {
//      stopLampIdleModeRun = "true";
//      stopLampIdleModeRun = "true";
//  }else{
//      stopLampOffModeRun = "true";
//      stopLampOffModeRun = "true";
//  }

  if(isSensorLoopActive == 1) {
//    if(sensorMalfunctionLamp == 1 && headShortLampActive == "true") {
//      headShortLampActive = "true";
//      headLongLampActive = "true";
//    }else{
//      headShortLampActive = "false";
//      headLongLampActive = "false";
//    }
//
//    if(sensorMalfunctionLamp == 1 && headShortLampActive == "true" && headLongLampActive == "true") headLongLampActive = "true";
//    else headLongLampActive = "false";
    
    Serial.print("sensor: ");
    Serial.println(sensorMalfunctionLamp[0]);
//    isSensorLoopActive = 0;
  }
//  headShortLampActive headLongLampActive
//  sensorMalfunctionLamp = digitalRead(pinSensorMalfunctionLamp);
//  if(sensorMalfunctionLamp != 1 &&  (headShortLampActive == "true" || headLongLampActive == "true") {
////    Serial.println("GOOD");
//  }else if(sensorMalfunctionLamp == 0 && c == 1) {
////    Serial.println("malfunction");
//  }else{
////  Serial.print("BAD: ");
////      Serial.print("sensor: ");
////      Serial.print(sensorRead);
////      Serial.print(" ");
////      Serial.print("lamp: ");
////      Serial.println(c);
//  }
}

//===================================================================

void loop() {
   malfunctionlamp();
   
   runled();
   seinsistem();
   cleanlamp();

   idleMode();
   startMode();
   offMode();
   //  ultras();
}
