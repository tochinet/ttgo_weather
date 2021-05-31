#include "userinfo.h"
#include "ani.h"
#include "ez10d.h" // day
#include "ez01n.h" // night
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <ArduinoJson.h>         //https://github.com/bblanchon/ArduinoJson.git
#include <NTPClient.h>           //https://github.com/taranais/NTPClient

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB
#define lightblue 0x01E9
#define darkred 0xA041
#define blue 0x5D9B
#include "Orbitron_Medium_20.h"
#include <WiFi.h>

#include <WiFiUdp.h>
#include <HTTPClient.h>

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;


//const char* ssid     = ""; // not use - blynk 
//const char* password = ""; // not use - blynk 

extern const char ssid[];
extern const char password[];

/**
 *  you can get info (town,country,key,gps)
 *  https://openweathermap.org/api
 *
 **/

String town="Hwawŏn";        // modify user location
String Country="KR";         // modify user country
//const String key = "";     // modify user api key

const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q="+town+","+Country+"&units=metric&APPID=";

// modify user gps location(lat/lon) below
const String airpollution = "http://api.openweathermap.org/data/2.5/air_pollution?lat=35.75032680073246&lon=128.36178126630787&appid=";

String payload=""; //whole json 
String tmp="" ; //temperatur
String hum="" ; //humidity
String wspeed="";
String icon="";
String getpoll="";  
String pol25="";
String pol10="";


StaticJsonDocument<1000> doc;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

int backlight[5] = {0,30,60,120,220};
byte b=1;

void setup(void) {
  pinMode(0,INPUT_PULLUP);
  pinMode(35,INPUT);

   
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextSize(1);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, backlight[b]);

  Serial.begin(115200);
  tft.print("Connecting to ");
  tft.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    tft.print(".");
  }
  
  tft.println("");
  tft.println("WiFi connected.");
  tft.println("IP address: ");
  tft.println(WiFi.localIP());
  delay(3000);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  

        
//  tft.setCursor(2, 232, 1);
//  tft.println(WiFi.localIP());
  tft.setCursor(80, 199, 2);
  tft.println("AIR");
  tft.setCursor(80,97,2);
  tft.println("SPD m/s");
  
//  tft.setCursor(4,90,2);
//  tft.println("STAT");
//  
  
  
  tft.setCursor(80, 146, 2);
  tft.println("SEC");
  tft.setTextColor(TFT_WHITE,lightblue);
  tft.setCursor(4, 146, 2);
  tft.println("TEMP");


//tft.pushImage(70, 88,  60, 60, ez13d);
  
  tft.setCursor(4, 199, 2);
  tft.println("HUM: ");
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  
  tft.setFreeFont(&Orbitron_Medium_20);
  tft.setCursor(6, 82);
//           tft.println(town); // 지역
  tft.println(dayStamp); // 지역
  
  
//  tft.fillRect(68,152,1,74,TFT_GREY);
  tft.fillRect(68,99,1,144,TFT_GREY);
  
//  for(int i=0;i<b+1;i++)
//  tft.fillRect(78+(i*7),216,3,10,blue);

// Initialize a NTPClient to get time
  timeClient.begin(); 
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(32400);   /*EDDITTTTTTTTTTTTTTTTTTTTTTTT                      */
  getData();
  delay(500);
}
int i=0;
String tt="";
int count=0;
bool inv=1;
int press1=0; 
int press2=0;////

int frame=0;
String curSeconds="";

int lmod = 0; // 모드 0,1

int hh = 0;
int mm = 0;
String apm = "";

unsigned long previousMillis  = 0;
unsigned long currentMillis   = 0;
int interval=1000;
int delay_time = -1;     // 지연시간(분)


void loop() {

  
  if(icon=="01n"){
    tft.pushImage(0, 89,  60, 60, ez01n);  
  }else if(icon=="02n"){
    tft.pushImage(0, 89,  60, 60, ez02n);
  }else if(icon=="03n"){
    tft.pushImage(0, 89,  60, 60, ez03n);
  }else if(icon=="04n"){
    tft.pushImage(0, 89,  60, 60, ez04n);
  }else if(icon=="09n"){
    tft.pushImage(0, 89,  60, 60, ez09n);
  }else if(icon=="10n"){
    tft.pushImage(0, 89,  60, 60, ez10n);
  }else if(icon=="11n"){
    tft.pushImage(0, 89,  60, 60, ez11n);
  }else if(icon=="13n"){
    tft.pushImage(0, 89,  60, 60, ez13n);
  }else if(icon=="50n"){
    tft.pushImage(0, 89,  60, 60, ez50n);
  }else if(icon=="01d"){
    tft.pushImage(0, 89,  60, 60, ez01d);
  }else if(icon=="02d"){
    tft.pushImage(0, 89,  60, 60, ez02d);
  }else if(icon=="03d"){
    tft.pushImage(0, 89,  60, 60, ez03d);
  }else if(icon=="04d"){
    tft.pushImage(0, 89,  60, 60, ez04d);
  }else if(icon=="09d"){
    tft.pushImage(0, 89,  60, 60, ez09d);
  }else if(icon=="10d"){
    tft.pushImage(0, 89,  60, 60, ez10d);
  }else if(icon=="11d"){
    tft.pushImage(0, 89,  60, 60, ez11d);
  }else if(icon=="13d"){
    tft.pushImage(0, 89,  60, 60, ez13d);
  }else if(icon=="50d"){
    tft.pushImage(0, 89,  60, 60, ez50d);
  }


  
//  tft.pushImage(70, 88,  60, 60, ez13d);

  if((unsigned long)(millis() - previousMillis) >= interval)
  {
    Serial.println("------------>switch");
    // 시간되 었을때 수행할 내용
    inv=!inv;
    snapshotTime(5);
  }



   if(digitalRead(35)==0){
    Serial.println("---------------------->click 1 오른쪽 버턴 클릭");
     if(press2==0){
      press2=1;
      tft.fillRect(78,216,44,12,TFT_BLACK);
   
      b++;
      if(b>=5)
        b=0;
  
      ledcWrite(pwmLedChannelTFT, backlight[b]);
     }
   }else press2=0;

   if(digitalRead(0)==0){
    
   
     if(press1==0){
      press1=1;
      
      Serial.println("---------------------->click 2 왼쪽 버튼 클릭");
     }
   }else press1=0;
   
   if(count==0)
    getData();
   count++;
   if(count>2000)
    count=0;


/**
 * 0번모드 출력
 *  head - pm2.5, 온도
 */
// tft.setCursor(4, 199, 2);
//  tft.setCursor(80, 199, 2);
//  tft.println("PM 2.5");

    tft.setTextFont(1);
    tft.setCursor(105, 204);
    if(inv){
      tft.println("pm 25");
      int ipm25 = pol25.toInt();
      if(ipm25>15 && ipm25 <36){
        // 보통
        tft.fillRect(78,220,65,28,TFT_YELLOW);
        tft.setTextColor(TFT_BLACK);
      }else if(ipm25>35 && ipm25<76){
        // 나쁨
        tft.fillRect(78,220,65,28,TFT_ORANGE);
        tft.setTextColor(TFT_BLACK);
      }else if(ipm25>75){
        // 매우 나쁨
        tft.fillRect(78,220,65,28,TFT_RED);
        tft.setTextColor(TFT_WHITE);
      
      }else{
        tft.fillRect(78,220,65,28,TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
      }

    }else{
      tft.println("pm 10");
      int ipm10 = pol10.toInt();
      if(ipm10>30 && ipm10 <81){
        // 보통
        tft.fillRect(78,220,65,28,TFT_YELLOW);
        tft.setTextColor(TFT_BLACK);
      }else if(ipm10>80 && ipm10<151){
        // 나쁨
        tft.fillRect(78,220,65,28,TFT_ORANGE);
        tft.setTextColor(TFT_BLACK);
      }else if(ipm10>150){
        // 매우 나쁨
        tft.fillRect(78,220,65,28,TFT_RED);
        tft.setTextColor(TFT_WHITE);
      
      }else{
        tft.fillRect(78,220,65,28,TFT_BLACK);
        tft.setTextColor(TFT_WHITE);
      }
      

    }

    tft.setFreeFont(&Orbitron_Light_24); 
    tft.setCursor(79, 239);

    if(inv){
      tft.println(pol25);
    }else{
      tft.println(pol10);
    }
    tft.setTextColor(TFT_WHITE);
/**
 * 1번모드 출력
 * HEAD - pm10, 습도
 */

   
//    tft.setFreeFont(&Orbitron_Medium_20);
    tft.fillRect(1,166,48,28,TFT_BLACK);
    tft.setFreeFont(&Orbitron_Light_24); 
    tft.setCursor(2, 186);
    tft.println(tmp.substring(0,3));

    tft.fillRect(1,220,65,28,TFT_BLACK);
    tft.setCursor(2, 239);
    tft.println(hum+"%");

    tft.fillRect(78,116,48,28,TFT_BLACK);
    tft.setCursor(79, 136);
//    tft.println(curSeconds);      // ------------------------------------------------------------------
    tft.println(wspeed);
    
    tft.setTextColor(TFT_ORANGE,TFT_BLACK);
    tft.setTextFont(4);
    tft.setCursor(6, 44);
    tft.println(dayStamp);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);



    tft.setFreeFont(&Orbitron_Medium_20);
    tft.setCursor(6, 88);
    tft.println(town); // 지역
          
  
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
//  Serial.println(formattedDate);

 
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
 
 
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
         
  if(curSeconds!=timeStamp.substring(6,8)){
  // 초 출력
   tft.fillRect(78,164,48,28,darkred);
   tft.setFreeFont(&Orbitron_Light_24);                   
   tft.setCursor(81, 186);
   tft.println(timeStamp.substring(6,8));
   curSeconds=timeStamp.substring(6,8);
  }
  
  tft.setFreeFont(&Orbitron_Light_32);
  
  
  String current=timeStamp.substring(0,5);
  if(current!=tt)
  {
    // 시간 출력
    Serial.print("시간:");
    Serial.println(timeClient.getHours());
  
    hh = timeClient.getHours();
    mm = timeClient.getMinutes(); 
  
    tft.fillRect(3,8,120,30,TFT_BLACK);
    tft.setCursor(5, 34);
  //            tft.println(timeStamp.substring(0,5)); // 시간 출력
    
    if(hh > 12)
    {
      hh = hh - 12;
      Serial.println(chNF2(hh));
      tft.print(chNF2(hh));
      tft.print(":");
      tft.print(chNF2(mm));
      apm = "PM";
    }else
    {
      tft.print(chNF2(hh));
      tft.print(":");
      tft.print(chNF2(mm));
      apm = "AM";
    }
    
  
    
    tft.setTextFont(2);
    tft.setCursor(111, 21);
    tft.println(apm);
    
    tt=timeStamp.substring(0,5);
  }

  delay(80);
}

String rtn = "";

String chNF2(int num)
{
  rtn = "";
  if(num < 10)
  {
    rtn += '0';
    rtn += num;
  }else
  {
    rtn += num;
  }
  return rtn;
}


void getData()
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
      HTTPClient http;
      http.begin(endpoint + key); //Specify the URL
      int httpCode = http.GET();  //Make the request
   
      if (httpCode > 0) { //Check for the returning code
        payload = http.getString();
        // Serial.println(httpCode);
        Serial.println(payload);
      }else {
        Serial.println("Error on HTTP request");
      }
      http.end(); //Free the resources

      /**
       * 미세먼지값 불러 오기
       */
      http.begin(airpollution + key); //Specify the URL
      httpCode = http.GET();  //Make the request
   
      if (httpCode > 0) { //Check for the returning code
        getpoll = http.getString();
        // Serial.println(httpCode);
        Serial.println(getpoll);
      }else {
        Serial.println("Error on HTTP request");
      }
      http.end(); //Free the resources

      // 
    }
    
    char inp[1000];
    payload.toCharArray(inp,1000);
    deserializeJson(doc,inp);

    String tmp2 = doc["main"]["temp"];
    String hum2 = doc["main"]["humidity"];
    String town2 = doc["name"];
    String icon2 = doc["weather"][0]["icon"];
    String spd  = doc["wind"]["speed"];
    
    tmp=tmp2;
    hum=hum2;
    icon=icon2;
    wspeed=spd.substring(0,spd.indexOf("."));

    Serial.println("Temperature"+String(tmp));
    Serial.println("Humidity"+hum);
    Serial.println("icon:"+icon);
    Serial.println("Wind Speed:"+wspeed);
    
    

    /**
     * 미세먼지 추출
     */
    char inp2[1000];
    getpoll.toCharArray(inp2,1000);
    deserializeJson(doc,inp2);
//Serial.println(""+doc);
//    String p25 = doc["components"]["pm2_5"];


String output;
serializeJson(doc, output);
Serial.println(output);


    String p25 = doc["list"][0]["components"]["pm2_5"];
    String p10 = doc["list"][0]["components"]["pm10"];
    pol25=p25;
    pol10=p10;
    Serial.println("p2.5:"+String(pol25));
    Serial.println("p10:"+pol10);
     
}

/**
 * Delay Time Set
 * 단위 : 분
 */
void snapshotTime(int delay_time)
{
  previousMillis = millis();
  interval = delay_time*1000;
}

         


     
