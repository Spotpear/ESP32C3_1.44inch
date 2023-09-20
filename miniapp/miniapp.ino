//屏幕驱动库
#include <TFT_eSPI.h>
//spi驱动库
#include <SPI.h>
//图片解码库
#include <TJpg_Decoder.h>
//json库,用于网页解析
#include "ArduinoJson.h"
//时间库,用于时钟部分
#include <TimeLib.h>
//wifi库
#include <WiFi.h>
//http请求库
#include <HTTPClient.h>
//创建wifi的udp服务的库
#include <WiFiUdp.h>
//静态存储区,用于存储一些固定参数
 #include <EEPROM.h>

#include <Arduino.h>
#include "lvgl.h"
//天气图标
#include "weathernum.h"

//温湿度图标
#include "img/temperature.h"
#include "img/humidity.h"

//字体文件用于时钟显示
#include "font/FxLED_32.h"
//字体文件用于星期和日月,粉丝数显示
#include "font/zkyyt12.h"
//字体文件用于城市名字显示
#include "font/city10.h"
//字体文件用于天气显示
#include "font/tq10.h"
//#include "font/AAA.h"
#include "font/ALBB10.h"


//此处添加WIFI信息或者个人热点WLAN信息
const char *ssid     = "";  //Wifi名字 
const char *password = ""; //Wifi密码 

// const char *ssid     = "spotpear";  //Wifi名字 
// const char *password = "12345678"; //Wifi密码 



WeatherNum  wrat; //天气对象
int prevTime = 0;
int AprevTime = 0;
int Anim = 0;
uint32_t targetTime = 0;


//背景颜色
uint16_t bgColor =0x0000 ;

//字体颜色

//时间小时分钟字体颜色
uint16_t timehmfontColor =0xFFFF ;
//时间秒字体颜色
uint16_t timesfontColor =0xFFFF ;
//星期字体颜色
uint16_t weekfontColor =0xFFFF ;
//日月字体颜色
uint16_t monthfontColor =0xFFFF ;
//温湿度字体颜色
uint16_t thfontColor =0xFFFF ;
//左上角轮播字体颜色
uint16_t tipfontColor =0xFFFF ;
//城市名称字体颜色
uint16_t cityfontColor =0xFFFF ;
//空气质量字体颜色
uint16_t airfontColor =0xFFFF ;
//b站粉丝数字体颜色
uint16_t bilifontColor =0xF81F ;




//线框颜色

uint16_t  xkColor= 0xFFFF;

//lvgl
static const uint16_t screenWidth = 128;
static const uint16_t screenHeight = 128;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenHeight * screenWidth / 10];





//时钟部分参数---------------------------------
//NTP服务器
static const char ntpServerName[] = "time.nist.gov"; //NTP服务器
float timeZone;     //时区



static String cityCode = "";  //如果需要改为 手动添加城市信息 则在""内填写城市的名字，然后将第218行代码注释掉即可
char* api_key = "fbf5a0e942e6fea3ff18103b9fd46ed9"; //API 密钥


WiFiUDP Udp;
unsigned int localPort = 8000;
WiFiClient wificlient;


time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
String num2str(int digits);
void sendNTPpacket(IPAddress &address);

byte setNTPSyncTime = 20; //设置NTP时间同步频率，10分钟同步一次

int backLight_hour=0;

time_t prevDisplay = 0; // 显示时间


//-----------------------------------------

//----------------------温湿度等参数------------------
unsigned long wdsdTime = 0;
byte wdsdValue = 0;
String wendu1 = "",wendu2 = "",shidu = "",yaqiang = "",tianqi = "",kjd = "";

//----------------------------------------------------

LV_IMG_DECLARE(TKR_A);
static lv_obj_t *logo_imga = NULL;


TFT_eSPI tft = TFT_eSPI(128,128);  // 引脚请自行配置tft_espi库中的 User_Setup.h文件
TFT_eSprite clk = TFT_eSprite(&tft);



bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{

  if ( y >= tft.height() ) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  // Return 1 to decode next block
  return 1;

}


void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}


void tkr(void)
{
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenHeight * screenWidth / 10);
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = 128;
  disp_drv.ver_res = 128;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;  
  lv_disp_drv_register(&disp_drv);

  static lv_style_t style;  
  lv_style_init(&style);
  lv_style_set_bg_color(&style, lv_color_black());
  lv_obj_add_style(lv_scr_act(), &style, 0);

  logo_imga = lv_gif_create(lv_scr_act());
  lv_obj_center(logo_imga);
  lv_obj_align(logo_imga, LV_ALIGN_LEFT_MID, 50, 0);
  lv_gif_set_src(logo_imga, &TKR_A);

  lv_timer_handler();
  delay(1);
}


//-----------------------------------城市代码获取及展示
//获取城市信息ID
void getCityCode() {
 
String URL = "http://ip-api.com/json/?fields=city,lat,lon";

HTTPClient httpClient;
httpClient.begin(wificlient,URL);


//启动连接并发送HTTP请求
  int httpCode = httpClient.GET();
  Serial.print("Send GET request to URL: ");
  Serial.println(URL);

  if (httpCode == HTTP_CODE_OK) {
    
    DynamicJsonDocument doc(1024);
    deserializeJson(doc,httpClient.getString());
    String city = doc["city"];

    cityCode=city; //获取当地城市ID
  }

  httpClient.end();

}



void getCityTime(){
  getCityCode();
  String URL = "http://api.openweathermap.org/data/2.5/weather?q=" + cityCode + "&appid=" + String(api_key) + "&units=metric"; //524901
  HTTPClient httpClient;
  httpClient.begin(URL);

  int httpCode = httpClient.GET();
   if (httpCode == HTTP_CODE_OK) {
      const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(40) + 470;
      DynamicJsonDocument doc(capacity);
      deserializeJson(doc,httpClient.getString());

      long timezone1 = doc["timezone"]; //获取时区信息
      timeZone=(float)(timezone1/3600);

      Serial.println("获取城市信息成功");
   }else {
    Serial.println("获取城市信息失败");
    Serial.print(httpCode);
  }
  httpClient.end();
}

int temp_i1,temp_i2;
 String scrollText[5];
// 获取城市天气 fc_24_en  1694222530090  401070101 fbf5a0e942e6fea3ff18103b9fd46ed9
void getCityWeater(){

  float temp_f,temp_min_f,temp_max_f;
  getCityCode();
  String URL = "http://api.openweathermap.org/data/2.5/weather?q=" + cityCode + "&appid=" + String(api_key) + "&units=metric"; //524901
 
  //创建 HTTPClient 对象
 HTTPClient httpClient;
 httpClient.begin(URL);


  int httpCode = httpClient.GET();
  Serial.println("正在获取天气数据");
  Serial.println(URL);

  //如果服务器响应OK则从服务器获取响应体信息并通过串口输出
  if (httpCode == HTTP_CODE_OK) {

    const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(40) + 470;
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc,httpClient.getString());
    // JsonObject sk = doc.as<JsonObject>();
    // String str1 = httpClient.getString();
 
    float temp = doc["main"]["temp"]; // Temperature in Celsius
    int humidity = doc["main"]["humidity"]; // Humidity in percentage
    int pressure = doc["main"]["pressure"];
    String description = doc["weather"][0]["description"]; // Weather description 天气
    String icon = doc["weather"][0]["icon"];
    float temp_min = doc["main"]["temp_min"];
    float temp_max = doc["main"]["temp_max"];
    int visibility = doc["visibility"];
    
    temp_f = 32 + temp*1.8;
    temp_min_f = 32 + temp_min*1.8;
    temp_max_f = 32 + temp_max*1.8;

    temp_i1 = int(temp_f);
    temp_i2 = int(temp);

    wendu1 = String(temp_i1);//温度 ℉
    wendu2 = String(temp_i2); //温度 ℃
    shidu = String(humidity); //湿度
    yaqiang = String(pressure); //压强
    tianqi = String(description);//天气
    kjd = String(visibility/1000); //可见度
    
    clk.setColorDepth(8);
    clk.loadFont(ALBB10);
  
    // shidu = sk["SD"].as<String>();

  //城市名称
  clk.createSprite(77, 16);
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(cityfontColor, bgColor);
  clk.drawString(cityCode,1,8); //
  clk.pushSprite(1,89);
  clk.deleteSprite();
  clk.unloadFont();
  // temp=26;
  uint16_t pm25BgColor = tft.color565(156, 202, 127);
  if(temp<10)
    pm25BgColor = tft.color565(0,0,255);//蓝色
  else if(temp<20 && temp>=10)
    pm25BgColor = tft.color565(46,185,201);//淡蓝色
  else if(temp>=20 && temp<=25)
    pm25BgColor = tft.color565(156, 202, 127); //绿色
  else if(temp>25 && temp<30)
    pm25BgColor = tft.color565(222, 202, 24); //黄色
  else if(temp>=30)
    pm25BgColor = tft.color565(136, 11, 32);//红色
  
  clk.setColorDepth(8);
  clk.loadFont(ALBB10);
  clk.createSprite(36,15);
  clk.fillSprite(bgColor);
  clk.fillRoundRect(0, 0, 32, 15, 4, pm25BgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(airfontColor);
  clk.drawString("Temp", 3, 7);

  clk.pushSprite(93, 69);
  clk.deleteSprite();
  clk.unloadFont();

  scrollText[0] = "MIN T "+String(temp_min_f)+ "℉ / "+String(temp_min)+"℃";
  scrollText[1] = "MAX T "+String(temp_max_f)+ "℉ / "+String(temp_max)+"℃";
  scrollText[2] = "Weather "+String(tianqi);
  scrollText[3] = "pressure "+String(yaqiang)+"hPa";
  scrollText[4] = "visibility "+String(kjd)+"km";
  wrat.printfweather1(1,47,icon);

  Serial.println("获取成功");

  } else {
    Serial.println("请求城市天气错误：");
    Serial.print(httpCode);
  }

  //关闭ESP32与服务器连接
  httpClient.end();
}

//---------------- 温湿度轮播显示------------------------------------------

void weatherWarning() { //间隔5秒切换显示温度和湿度，该数据为气象站获取的室外参数
  if(millis() - wdsdTime > 5000) {
    wdsdValue = wdsdValue + 1;
    //Serial.println("wdsdValue0" + String(wdsdValue));
    clk.setColorDepth(8);
    clk.loadFont(ALBB10);
    switch(wdsdValue) {
      case 1:
      //Serial.println("wdsdValue1" + String(wdsdValue));
        TJpgDec.drawJpg(82,89,temperature, sizeof(temperature));  //温度图标
        for(int i=20;i>0;i--) {
          clk.createSprite(30,16);
          clk.fillSprite(bgColor);
          clk.setTextDatum(ML_DATUM);
          clk.setTextColor(thfontColor, bgColor);
          clk.drawString(wendu1+"℉",1,i+8); //AW wendu+
          clk.pushSprite(98,89);
          clk.deleteSprite();
          delay(10);
        }
        break;
      case 2:
      //Serial.println("wdsdValue1" + String(wdsdValue));
        TJpgDec.drawJpg(82,89,temperature, sizeof(temperature));  //温度图标
        for(int i=20;i>0;i--) {
          clk.createSprite(30,16);
          clk.fillSprite(bgColor);
          clk.setTextDatum(ML_DATUM);
          clk.setTextColor(thfontColor, bgColor);
          clk.drawString(wendu2+"℃",1,i+8); //AW wendu+
          clk.pushSprite(98,89);
          clk.deleteSprite();
          delay(10);
        }
        break;
      case 3:
      //Serial.println("wdsdValue2" + String(wdsdValue));
        TJpgDec.drawJpg(82,89,humidity, sizeof(humidity));  //湿度图标
        for(int i=20;i>0;i--) {
          clk.createSprite(30, 16);
          clk.fillSprite(bgColor);
          clk.setTextDatum(ML_DATUM);
          clk.setTextColor(thfontColor, bgColor);
          clk.drawString(shidu+"%",1,i+8);
          clk.pushSprite(98,89);
          clk.deleteSprite();
          delay(10);
        }
        wdsdValue = 0;
        break;
    }
    wdsdTime = millis();
    clk.unloadFont();
  }
}


//----------------------------左上角天气信息轮播-------------------------
int currentIndex = 0;

TFT_eSprite clkb = TFT_eSprite(&tft);

void scrollBanner(){
  if(millis() - prevTime > 3500){ //3.5秒切换一次

    if(scrollText[currentIndex]){

      clkb.setColorDepth(8);
      clkb.loadFont(ALBB10);

      for(int pos = 20; pos>0 ; pos--){
        scrollTxt(pos);
      }

      clkb.deleteSprite();
      clkb.unloadFont();

      if(currentIndex>=4){
        currentIndex = 0;  //回第一个
      }else{
        currentIndex += 1;  //准备切换到下一个
      }

    }
    prevTime = millis();

  }
}

void scrollTxt(int pos){
    clkb.createSprite(128, 16);
    clkb.fillSprite(bgColor);
    clkb.setTextWrap(false);
    clkb.setTextDatum(ML_DATUM);
    clkb.setTextColor(tipfontColor, bgColor);
    clkb.drawString(scrollText[currentIndex],1,pos+8);
    clkb.pushSprite(1,1);
}

//----------------------------------------------




byte loadNum = 6;
void loading(byte delayTime)//绘制进度条
{
  clk.setColorDepth(8);
  clk.createSprite(100, 100);//创建窗口
  clk.fillSprite(0x0000);   //填充率

  clk.drawRoundRect(0,0,100,16,8,0xFFFF);       //空心圆角矩形
  clk.fillRoundRect(3,3,loadNum,10,5,0xFFFF);   //实心圆角矩形
  clk.setTextDatum(CC_DATUM);   //设置文本数据
  clk.setTextColor(TFT_GREEN, 0x0000);
  clk.drawString("Getting WiFi...",50,40,2);
  clk.setTextColor(TFT_WHITE, 0x0000);
  clk.drawRightString("MiNiApp V1.0",100,60,2);
  clk.pushSprite(14,40);  //窗口位置

  //clk.setTextDatum(CC_DATUM);
  //clk.setTextColor(TFT_WHITE, 0x0000);
  //clk.pushSprite(130,180);

  clk.deleteSprite();
  loadNum += 1;
  delay(delayTime);
}


void get_wifi()
{
    Serial.begin(115200);
  // 连接网络
  WiFi.begin(ssid, password);
  //等待wifi连接
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); //连接成功
  Serial.print("IP address: ");    //打印IP地址
  Serial.println(WiFi.localIP());
}


//---------------------------------------------------------------------------
void setup() {

         tkr();
         Serial.begin(115200);
         EEPROM.begin(1024);



         tft.begin(); /* TFT init */
         tft.invertDisplay(0);//反转所有显示颜色：1反转，0正常
         tft.fillScreen(0x0000);
         tft.setTextColor(TFT_WHITE, 0x0000);
         // 设置屏幕显示的旋转角度，参数为：0, 1, 2, 3
        // 分别代表 0°、90°、180°、270°
        //根据实际需要旋转
          tft.setRotation(2);




        tft.setCursor(0,8,1);
        // 设置文本颜色为白色，黑色文本背景
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        // 设置显示的文字，注意这里有个换行符 \n 产生的效果
        tft.println("---------------------");
        tft.println("Configure mobile WLAN information:");
        tft.println("WIFI name:spotpear");
        tft.println("WIFI password:12345678\n---------------------");
        tft.println("If there is a city information error,execute==>After turning on the phone WLAN,turn off the phone data,and then connect to the router WIFI.");
        tft.println("---------------------");
          

          TJpgDec.setJpgScale(1);
          TJpgDec.setSwapBytes(true);
          TJpgDec.setCallback(tft_output);

          targetTime = millis() + 1000;
          get_wifi();
          Serial.print("正在连接WIFI ");
          // Serial.println(ssid);
          //WiFi.begin(ssid,password);

          TJpgDec.setJpgScale(1);
          TJpgDec.setSwapBytes(true);
          TJpgDec.setCallback(tft_output);

          while (WiFi.status() != WL_CONNECTED)
          {
            loading(70);

            if(loadNum>=94)
            {
              // SmartConfig();
              break;
            }
          }
          tft.fillScreen(TFT_BLACK);
          while(loadNum < 94) //让动画走完
          {
            loading(1);
          }
          loading(1);
        
          Udp.begin(localPort);

          setSyncProvider(getNtpTime);
          setSyncInterval(setNTPSyncTime*60); //NTP网络同步频率，单位秒。


          //绘制一个视口

          tft.fillScreen(0x0000);
          tft.fillRoundRect(0,0,128,128,0,bgColor);//实心矩形


          //绘制线框
          tft.drawFastHLine(0,0,128,xkColor);


          tft.drawFastHLine(0,18,128,xkColor); //x:0 y:18 长度为:128
          tft.drawFastHLine(0,106,128,xkColor);

          // tft.drawFastVLine(80,0,18,xkColor);

          tft.drawFastHLine(0,88,128,xkColor);

          // tft.drawFastVLine(32,88,18,xkColor);
          tft.drawFastVLine(78,88,18,xkColor);

          tft.drawFastVLine(40,106,20,xkColor);
          tft.drawFastHLine(0,127,128,xkColor);
          
          getCityWeater();
          
}

unsigned long weaterTime = 0;
void loop() {
  
                if (now() != prevDisplay) {
                prevDisplay = now();
                digitalClockDisplay();
              }

             if(millis() - weaterTime > 300000){ //5分钟更新一次天气
                weaterTime = millis();
                getCityWeater();
                // get_Bstation_follow();
                // fanspush();
              }
    digitalClockDisplay();
     scrollBanner();
     weatherWarning();
    lv_timer_handler();
    delay(1);
    //  Serial_set();
  }


//时钟显示函数--------------------------------------------------------------------------


void digitalClockDisplay()
{

  clk.setColorDepth(8);

  /***中间时间区***/
  //时分
  clk.createSprite(75, 28);
  clk.fillSprite(bgColor);
  clk.loadFont(FxLED_32);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(timehmfontColor, bgColor);
  clk.drawString(hourMinute(),1,14,7); //绘制时和分
  clk.unloadFont();
  clk.pushSprite(10,19);
  clk.deleteSprite();

  //秒
  clk.createSprite(50, 28);
  clk.fillSprite(bgColor);

  clk.loadFont(FxLED_32);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(timesfontColor, bgColor);
  clk.drawString(":"+num2str(second()),1,14);

  clk.unloadFont();
  clk.pushSprite(86,19);
  clk.deleteSprite();
  /***中间时间区***/

  /***底部***/
  clk.loadFont(ALBB10);
  clk.createSprite(70, 16);
  clk.fillSprite(bgColor);

  //星期
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(weekfontColor, bgColor);
  clk.drawString(week(),1,8);
  clk.pushSprite(45,108); // 字体的位置
  clk.deleteSprite();

  // 月日
  clk.createSprite(30,16);//框的大小
  clk.fillSprite(bgColor);
  clk.setTextDatum(ML_DATUM);
  clk.setTextColor(monthfontColor, bgColor);
  clk.drawString(monthDay(),1,8);
  clk.pushSprite(1,108);
  clk.deleteSprite();

  clk.unloadFont();
  /***底部***/

}

//星期
String week(){
  String wk[7] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
  String s = wk[weekday()-1];
  return s;
}

//月日
String monthDay(){
  String s = String(month());
  s = s + " - " + day();
  return s;
}
//时分
String hourMinute(){
  String s = num2str(hour());
  backLight_hour = s.toInt();
  s = s + ":" + num2str(minute());
  return s;
}

String num2str(int digits)
{
  String s = "";
  delay(9); //调整时间的快慢
  if (digits < 10)
    s = s + "0";
  s = s + digits;
  return s;
}

void printDigits(int digits)
{
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
//------------------------------------------------------------------------------------





//NTP部分的代码,包含2个函数------------------------------------------------------------

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP时间在消息的前48字节中
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  getCityTime();
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  //Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  //Serial.print(ntpServerName);
  //Serial.print(": ");
  //Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("NTP同步成功");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      //Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  //ESP.restart(); //时间获取失败直接重启
  Serial.println("NTP同步失败");
  return 0; // 无法获取时间时返回0
}

// 向NTP服务器发送请求
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision  调整时间精度
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
//--------------------------------------------------------------------------
