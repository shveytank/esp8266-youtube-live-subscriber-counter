#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_ST7735.h>   // include Adafruit ST7735 TFT library

#include <YoutubeApi.h>
#include <ArduinoJson.h>

// ST7735 TFT module connections
#define TFT_RST   D4     // TFT RST pin is connected to NodeMCU pin D4 (GPIO2)
#define TFT_CS    D3     // TFT CS  pin is connected to NodeMCU pin D4 (GPIO0)
#define TFT_DC    D2     // TFT DC  pin is connected to NodeMCU pin D4 (GPIO4)

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//------- Replace the following! ------
char ssid[] = "xxxxxxx";       // your network SSID (name)
char password[] = "xxxxxxx";  // your network key
#define API_KEY "xxxxxxxxxxxxxxxxxxxxxxxxxx"  // your google apps API Token
#define CHANNEL_ID "your_channel_id" // makes up the url of channel
//------- ---------------------- ------

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long timeBetweenRequests = 10000;
unsigned long nextRunTime;

long subs = 0;
long views = 0;
long videos = 0;
String channelName="your channel name";
String sSubs="";
String sViews="";
String sVideos="";

const unsigned char yt[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000111,B11111111,B11111111,B11111111,B11111100,B00000000,
  B00001111,B11111111,B11111111,B11111111,B11111110,B00000000,
  B00011111,B11111111,B11111111,B11111111,B11111111,B00000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B01111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B01111111,B11111111,B10011111,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10001111,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10000011,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10000000,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10000000,B00111111,B11111111,B11000000,
  B01111111,B11111111,B10000000,B00011111,B11111111,B11000000,
  B01111111,B11111111,B10000000,B00011111,B11111111,B11000000,
  B01111111,B11111111,B10000000,B01111111,B11111111,B11000000,
  B01111111,B11111111,B10000000,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10000011,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10001111,B11111111,B11111111,B11000000,
  B01111111,B11111111,B10111111,B11111111,B11111111,B11000000,
  B01111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00111111,B11111111,B11111111,B11111111,B11111111,B10000000,
  B00011111,B11111111,B11111111,B11111111,B11111111,B00000000,
  B00001111,B11111111,B11111111,B11111111,B11111110,B00000000,
  B00000011,B11111111,B11111111,B11111111,B11111000,B00000000,
  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000
};


const unsigned char subscriber[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000011,B11001110,B00111000,B00000000,
  B00000111,B10111111,B10111100,B00000000,
  B00000111,B10111111,B11011100,B00000000,
  B00000111,B00111111,B11011100,B00000000,
  B00000111,B01111111,B11011100,B00000000,
  B00000111,B00111111,B11011100,B00000000,
  B00000011,B00111111,B11011100,B00000000,
  B00000011,B10111111,B11011000,B00000000,
  B00000001,B10111111,B10011000,B00000000,
  B00000011,B10111111,B10111100,B00000000,
  B00011111,B11011111,B10111111,B00000000,
  B00111111,B11011111,B01111111,B11000000,
  B01111111,B10011111,B00111111,B11100000,
  B01111100,B00111111,B11000111,B11100000,
  B11111001,B11111111,B11111001,B11100000,
  B00000111,B11111111,B11111100,B00000000,
  B00001111,B11111111,B11111110,B00000000,
  B00001111,B11111111,B11111111,B00000000,
  B00011111,B11111111,B11111111,B00000000,
  B00011111,B11111111,B11111111,B00000000,
  B00011111,B11111111,B11111111,B00000000,
  B00000000,B00000000,B00000000,B00000000
};

const unsigned char ytView[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00111110,B00000000,B00000000,
  B00000001,B11111111,B11000000,B00000000,
  B00000111,B11111111,B11110000,B00000000,
  B00001111,B11100011,B11111000,B00000000,
  B00011111,B10000000,B11111100,B00000000,
  B00111111,B00001000,B01111110,B00000000,
  B01111111,B00111110,B01111111,B00000000,
  B01111110,B00111110,B00111111,B00000000,
  B11111110,B01111111,B00111111,B10000000,
  B01111110,B00111110,B00111111,B00000000,
  B01111111,B00111110,B01111111,B00000000,
  B00111111,B00001000,B01111110,B00000000,
  B00011111,B10000000,B11111100,B00000000,
  B00001111,B11100011,B11111000,B00000000,
  B00000111,B11111111,B11110000,B00000000,
  B00000001,B11111111,B11000000,B00000000,
  B00000000,B01111110,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};


const unsigned char vid[] PROGMEM = {
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00001110,B00000000,B00000000,
  B00000000,B00011111,B10000000,B00000000,
  B00000000,B00111111,B10000000,B00000000,
  B00011110,B00111111,B11000000,B00000000,
  B00011111,B00111111,B11000000,B00000000,
  B00111111,B00111111,B11000000,B00000000,
  B00011111,B00101111,B10000000,B00000000,
  B00011110,B00011111,B00000000,B00000000,
  B00001110,B11001111,B01000000,B00000000,
  B01111111,B11111111,B11100000,B00000000,
  B01111111,B11111111,B11100000,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11111111,B00000000,
  B01111111,B11111111,B11100011,B00000000,
  B01111111,B11111111,B11100000,B00000000,
  B00000000,B00000000,B01000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000,
  B00000000,B00000000,B00000000,B00000000
};


void setup() {

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  // Required if you are using ESP8266 V2.5 or above
  client.setInsecure();
  
   tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
 tft.setRotation(1);
 
  // If you want to enable some extra debugging
  api._debug = true;

  tft.fillScreen(ST7735_BLACK);
}

void loop() {

  if (millis() > nextRunTime)  {
    if(api.getChannelStatistics(CHANNEL_ID))
    {
      subs = api.channelStats.subscriberCount;
      views = api.channelStats.viewCount;
      videos = api.channelStats.videoCount;
      if(subs>999999999)
      {
        long sub=subs/1000000000;
        long temp=subs%1000000000;
        temp=temp/100000000;
        sSubs=sub;
        if(temp>0)
        {
          sSubs=sSubs+".";
          sSubs=sSubs+(String)temp;
        }
        sSubs=sSubs+"B";
      }
      else if(subs>999999)
      {
        long sub=subs/1000000;
        long temp=subs%1000000;
        temp=temp/100000;
        sSubs=sub;
        if(temp>0)
        {
          sSubs=sSubs+".";
          sSubs=sSubs+(String)temp;
        }
        sSubs=sSubs+"M";
      }
      else if(subs>999)
      {
        long sub=subs/1000;
        long temp=subs%1000;
        temp=temp/100;
        sSubs=sub;
        if(temp>0)
        {
          sSubs=sSubs+".";
          sSubs=sSubs+(String)temp;
        }
        
        sSubs=sSubs+"K";
      }
      else
      {
        sSubs=subs;
      }

      if(views>999999999)
      {
        long view=views/1000000000;
        long temp=views%1000000000;
        temp=temp/100000000;
        sViews=view;
        if(temp>0)
        {
          sViews=sViews+".";
          sViews=sViews+(String)temp;
        }
        sViews=sViews+"B";
      }
      else if(views>999999)
      {
        long view=views/1000000;
        long temp=views%1000000;
        temp=temp/100000;
        sViews=view;
        if(temp>0)
        {
          sViews=sViews+".";
          sViews=sViews+(String)temp;
        }
        sViews=sViews+"M";
      }
      else if(views>999)
      {
        long view=views/1000;
        long temp=views%1000;
        temp=temp/100;
        sViews=view;
        if(temp>0)
        {
          sViews=sViews+".";
          sViews=sViews+(String)temp;
        }
        
        sViews=sViews+"K";
      }
      else
      {
        sViews=views;
      }

      if(videos>999)
      {
        long video=videos/1000;
        long temp=videos%1000;
        temp=temp/100;
        sVideos=video;
        if(temp>0)
        {
          sVideos=sVideos+".";
          sVideos=sVideos+(String)temp;
        }
        
        sVideos=sVideos+"K";
      }
      else
      {
        sVideos=videos;
      }
      
      Serial.println("---------Stats---------");
      Serial.print("Subscriber Count: ");
      Serial.println(subs);
      Serial.print("View Count: ");
      Serial.println(views);
      Serial.print("Video Count: ");
      Serial.println(videos);
      // Probably not needed :)
      //Serial.print("hiddenSubscriberCount: ");
      //Serial.println(api.channelStats.hiddenSubscriberCount);
      Serial.println("------------------------");
//      tft.fillScreen(ST7735_BLACK);
//      tft.print("Subscriber Count: ");
//      tft.print(sSubs);
//      tft.print("View Count: ");
//      tft.print(sViews);
//      tft.print("Video Count: ");
//      tft.print(videos);
display();
    }
    nextRunTime = millis() + timeBetweenRequests;
  }
  
}
void display()
{
  tft.fillScreen(ST7735_BLACK);
  tft.drawBitmap(58, 10, yt, 43, 30,ST7735_RED);
  tft.setCursor(35,50);
    tft.print(channelName);
  tft.drawBitmap(10, 70, subscriber, 28, 23,ST7735_WHITE); 
  tft.setCursor(8,105);
    tft.print(sSubs);
  tft.drawBitmap(67, 70, ytView, 25, 25,ST7735_WHITE); 
  tft.setCursor(66,105);
    tft.print(sViews);
  tft.drawBitmap(120, 70, vid, 25, 25,ST7735_WHITE); 
  tft.setCursor(122,105);
    tft.print(sVideos);
}
