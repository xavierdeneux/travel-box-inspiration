#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/Roboto.h>

// For the Adafruit shield, these are the default.
#define TFT_DC 2
#define TFT_CS 15
#define TFT_MOSI 13
#define TFT_CLK 14
#define TFT_RST 16
#define TFT_MISO 12
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


const char* ssid = "";
const char* password = "";


long lastMsg = 0; 

String lastFlight = "";
int flightInfosAreDisplayed = 0;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }  

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.setRotation(1);
    
}

void loop() {
  // Check WiFi Status
  if (WiFi.status() == WL_CONNECTED) {

    long now = millis();
  //Envoi d'un message par minute
  if (now - lastMsg > 1000 * 3) {
    Serial.print("Refresh");
    lastMsg = now;
    
    HTTPClient http;  //Object of class HTTPClient
    http.begin("http://flights.xavierdeneux.fr/");
    int httpCode = http.GET();
    //Check the returning code                                                                  
    if (httpCode > 0) {
      // Parsing
      const size_t capacity = JSON_ARRAY_SIZE(5) + 20*JSON_OBJECT_SIZE(11) + 730;
      DynamicJsonDocument doc(capacity);

      const char* json = "[{\"latitude\":45.7157,\"longitude\":5.0778,\"altitude\":853.265,\"speed\":209.2142,\"flight\":\"LBT545\",\"aircraft\":\"Airbus A320-214\",\"airline\":\"Nouvelair\",\"origin\":\"Nice\",\"destination\":\"Lyon\",\"distance\":15496},{\"latitude\":45.7157,\"longitude\":5.0832,\"altitude\":0,\"speed\":0,\"flight\":\"EZY75DY\",\"aircraft\":\"Airbus A319-111\",\"airline\":\"easyJet\",\"origin\":\"Lyon\",\"destination\":\"London\",\"distance\":15508},{\"latitude\":45.7141,\"longitude\":5.0838,\"altitude\":0,\"speed\":53.10822,\"flight\":\"EZY71RL\",\"aircraft\":\"Airbus A319-111\",\"airline\":\"easyJet\",\"origin\":\"Berlin\",\"destination\":\"Lyon\",\"distance\":15688},{\"latitude\":45.6659,\"longitude\":5.3643,\"altitude\":2977.4459006400484,\"speed\":337.9614,\"flight\":\"TJT1277\",\"aircraft\":\"Beech 1900D\",\"airline\":\"Twin Jet\",\"origin\":\"Lyon\",\"destination\":\"Zurich\",\"distance\":30819},{\"latitude\":45.7804,\"longitude\":5.6089,\"altitude\":11589.45443462359,\"speed\":683.9695,\"flight\":\"BAW559\",\"aircraft\":\"Airbus A319-131\",\"airline\":\"British Airways\",\"origin\":\"Rome\",\"destination\":\"London\",\"distance\":42339}]";
      deserializeJson(doc, http.getString());
      //deserializeJson(doc, json);
      JsonObject root_0 = doc[0];
      float root_0_latitude = root_0["latitude"]; // 45.7157
      float root_0_longitude = root_0["longitude"]; // 5.0778
      int root_0_altitude = root_0["altitude"]; // 0
      float root_0_speed = root_0["speed"]; // 20.92142
      const char* root_0_flight = root_0["flight"]; // "LBT545"
      const char* root_0_aircraft = root_0["aircraft"]; // "Airbus A320-214"
      const char* root_0_airline = root_0["airline"]; // "Nouvelair"
      const char* root_0_origin = root_0["origin"]; // "Nice"
      const char* root_0_destination = root_0["destination"]; // "Lyon"
      int root_0_distance = root_0["distance"]; // 15496

      
      
      if(String(root_0_flight) != ""){
        if(String(root_0_flight) != lastFlight){
          tft.fillScreen(ILI9341_BLACK);
          tft.fillRect(0,0, 320, 30, 0x5AEB);
          tft.drawLine(0,31,320,31, 0xFFFF);
    
          tft.fillRect(0,32, 320, 32, 0xAD75);
          tft.drawLine(0,63,320,63, 0xFFFF);
    
          
          tft.fillRect(0,215, 320, 240, 0x5AEB);
          tft.drawLine(0,214,320,214, 0xFFFF);          
        }
        
               
        lastFlight = String(root_0_flight);
                
        if(String(root_0_origin) == "Lyon"){
            //line1 = "Va a "+String(root_0_destination);
            displayOriginOrDestination("Va à");
            displayTown(root_0_destination);
        }else{
            //line1 = "Arrive de "+String(root_0_origin);
            displayOriginOrDestination("Arrive de");
            displayTown(root_0_origin);
        }

        displayTemperature();
        displayCompany(root_0_airline);
        displayAirplane(root_0_aircraft);
        displaySpeed(String(root_0_speed)+"km/h");
        displayAltitude("Alt. "+String(root_0_altitude)+"m");
        displayDistance("Dist. "+String(root_0_distance)+"m");
        flightInfosAreDisplayed = 1;
      }else{
        //Aucun vol
        if(flightInfosAreDisplayed == 1){
          tft.fillScreen(ILI9341_BLACK);
          flightInfosAreDisplayed = 0;
        }
        displayMsg("Aucun vol en cours");
      }
    }
      http.end();   //Close connection
    }
  }

  
  // Delay
  delay(10000);
}

void displayTemperature(){
    tft.fillCircle(270,135,24, 0x5AEB);
    tft.drawCircle(270,135,26, 0xFFFF);
    tft.setCursor(255,140);
    tft.setTextColor(ILI9341_WHITE);  
    tft.setFont(&FreeSerif9pt7b);
    tft.setTextSize(1);
    
    tft.print("16");
    tft.setCursor(275,133);
    tft.print("o");
    tft.setFont(&Roboto_Medium_16);
}

void displayCompany(String company){
  tft.setCursor(10,15);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setFont();
  tft.setTextSize(2);
  tft.println(company);
}

void displayAirplane(String airplane){
  tft.setCursor(10,40);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setFont();
  tft.setTextSize(2);
  tft.println(airplane);
}

void displayOriginOrDestination(String val){
  tft.setFont();
  tft.setCursor(10, 110);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setTextSize(2);
  tft.println(val);
}

void displayTown(String town){
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.setCursor(10,160);
  tft.setTextSize(2);
  tft.println(town); 
}

void displaySpeed(String speed){ 
  
  tft.fillRect(0,215, 130, 230, 0x5AEB);
  tft.setCursor(10,220);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setFont();
  tft.setTextSize(2);
  tft.println(speed);
}

void displayAltitude(String altitude){
  tft.fillRect(165,215, 50, 230, 0x5AEB);
  tft.setCursor(140,225);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setFont();
  tft.setTextSize(1);
  tft.println(altitude);
}

void displayDistance(String distance){
  tft.fillRect(255,215, 60, 230, 0x5AEB);
  tft.setCursor(220,225);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setFont();
  tft.setTextSize(1);
  tft.println(distance);
}

void displayMsg(String msg){
  tft.setFont(&FreeMonoBoldOblique12pt7b);
  tft.setCursor(30,130);
  tft.setTextSize(1);
  tft.println(msg); 
}
