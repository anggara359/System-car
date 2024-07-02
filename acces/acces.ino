#include <WiFi.h>
#include <Servo.h>

Servo myservo;  
int pos = 0;    
WiFiServer server(80);


const char *ssid = "AC-ESP32";
const char *passphrase = "12345678";

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

void setup()
{
  Serial.begin(115200);
  Serial.println();
   myservo.attach(13);

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,passphrase) ? "Ready" : "Failed!");
  //WiFi.softAP(ssid);
  //WiFi.softAP(ssid, passphrase, channel, ssdi_hidden, max_connection)
  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
client.println("Content-type:text/html");
client.println();

client.print("Click <a href=\"/H\">here</a> to turn ON.<br>");
client.print("Click <a href=\"/L\">here</a> to turn ON.<br>");
client.print("Click <a href=\"/zz\">here</a> camera.<br>");

client.println();

break ; 
}
else 
{
  currentLine ="";
}

}
else if (c != '\r') {
  currentLine +=c ; 
}
if (currentLine.endsWith("GET / H")){
digitalWrite(LED_BUILTIN,HIGH);
       for (pos = 0; pos <= 180; pos +=2) { 
    // in steps of 1 degree
    myservo.write(pos);             
    delay(15);     
              }
               for (pos = 180; pos >= 0; pos -=2) { 
    myservo.write(pos);            
    delay(15); 
               }
                for (pos = 0; pos <= 180; pos +=2) { 
    // in steps of 1 degree
    myservo.write(pos);             
    delay(15);     
              }
               for (pos = 180; pos >= 0; pos -=2) { 
    myservo.write(pos);            
    delay(15); 
               }
}
if (currentLine.endsWith("GET / L")){
digitalWrite(LED_BUILTIN,LOW);
}

}
}
client.stop();
Serial.println("Client Disconnected.");
}
}
