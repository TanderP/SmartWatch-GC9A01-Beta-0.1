#include <TFT_eSPI.h>
#include "logo.h"
#include "spotify logo 2 240x240.h"
#include "Espresif logo 240x240.h"
#include "Flower 240x240.h"
//#include <WiFi.h>
//#include <WiFiManager.h>

#define Button1 32
#define Button2 33
#define Button3 25
#define Button4 26
#define V_Inp 34
//wifimanager
//////#define ESP32_SSID "ESP32"
///WiFiManager wifiManager;
///bool connection = false;


bool timeout = true;
unsigned long timeoutTime;
//TFT
TFT_eSPI tft= TFT_eSPI();

TFT_eSprite sprite = TFT_eSprite(&tft);

// Variable define
bool Input1=false;
bool Input2=false;
bool Input3=false;
bool Input4=false;
uint32_t Batt_Lvl=0;

//Display Page Val
bool MainDP=true;

bool Refresh=false;

int textWidth;
int x;

//Display Page count
bool wait=true;
int Page IRAM_ATTR=0;
bool changePage=true; // page handler

// debug
int ramDisplay1;
int ramDisplay2;
int ramInp;

// Display menu
const int menuRowCount = 2;
const int menuColumnCount = 3;
const int menuCount = menuRowCount * menuColumnCount;
bool menuChange = true;
int selectedOption = 0;

const int menuItemSize = 54; // Smaller size for the menu items
const int menuItemSpacing = 7; // Space between menu items
const int menuItemCornerRadius = 5; // Rounded corner radius
const int highlightBorderWidth = 3; // Width of the highlight border
////Wifi menu
const int menu2Count = 3;
int selectedOption2 = 0;

bool selected = false;
bool WifiSubMenSelected=false;
bool WifiSubMenu=false;
// clock
volatile int seconds IRAM_ATTR= 0;
volatile int minutes IRAM_ATTR= 58;
volatile int hours IRAM_ATTR= 2;
volatile int days IRAM_ATTR= 21;
volatile int months IRAM_ATTR= 8;
volatile int years IRAM_ATTR= 2023;

volatile int days_Max IRAM_ATTR;
int daysOfWeekCount;
const char* daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* monthsOfYear[] = {"NULL","January", "February", "March", "April", "May", "June", "July", "August", "September", "Octobor", "November", "December"};

const unsigned long interval = 1000; 



// image
int imageW=240; // image size must be the same size with the creen or clsoe enough
int imageH=240;

int screenW=240;
int screenH=240;

///////////////////////////////////////////////////////////////////////////////////////////////////
void display1 (void * pvparameters);
void display2 (void * pvparameters);
void display3 (void * pvparameters);
void display4 (void * pvparameters);

void Inp_Read (void * pvparameters);
void function (void * pvparameters);



// wifi
void WifiConnect(void *PvParameter);
void TimeOut(void *PvParameter);
// VOlt read // Battery 
void VoltRead (void * pvparameters);
void onTimer(TimerHandle_t xTimer);


void setup() {
Serial.begin(115200);
pinMode(Button1,INPUT_PULLDOWN);
pinMode(Button2,INPUT_PULLDOWN);
pinMode(Button3,INPUT_PULLDOWN);
pinMode(Button4,INPUT_PULLDOWN);

pinMode(V_Inp,INPUT);

tft.init();
tft.setRotation(2);
tft.setSwapBytes(true);
tft.fillScreen(TFT_BLACK);
tft.pushImage(0,0,screenW,screenH,Tander_lgo_white_bg_compress__240x240);
delay(2000);
tft.pushImage(0,0,screenW,screenH,Espresif_logo_240x240);
delay(2000);

Refresh=true;
//tft.fillScreen(TFT_BLACK);
  TimerHandle_t timerHandle = xTimerCreate("timer", pdMS_TO_TICKS(interval), pdTRUE, 0, onTimer); // name, period, auto reload, timer id, callback
  if (timerHandle != NULL) {
    xTimerStart(timerHandle, 0);
  }
  
xTaskCreatePinnedToCore(display1, "Display", 3200, NULL, 2, NULL, 1); // class, name, stack, param, priority, handle, core
xTaskCreatePinnedToCore(display2, "Display2", 3200, NULL, 2, NULL, 1); // class, name, stack, param, priority, handle, core
xTaskCreatePinnedToCore(display3, "Display3", 6128, NULL, 2, NULL, 1); // class, name, stack, param, priority, handle, core
xTaskCreatePinnedToCore(display4, "Display4", 3200, NULL, 2, NULL, 1); // class, name, stack, param, priority, handle, core


xTaskCreatePinnedToCore(VoltRead, "VoltRead", 1048, NULL, 4, NULL, 0); // class, name, stack, param, priority, handle, core
xTaskCreatePinnedToCore(Inp_Read, "Button", 2128, NULL, 1, NULL, 0); // class, name, stack, param, priority, handle, core
xTaskCreatePinnedToCore(function, "function", 800, NULL, 3, NULL, 0); // class, name, stack, param, priority, handle, core

//wifimanager
///xTaskCreate(WifiConnect, "WifiConnect", 6125, NULL, 1, NULL); // class, name, stack, param, priority, handle, core
///xTaskCreate(TimeOut, "TimeOut", 4096, NULL, 1, NULL); // class, name, stack, param, priority, handle, core


}



void Inp_Read PROGMEM(void * pvparameters)
{while (1){
  bool wait = false;
  ramInp = uxTaskGetStackHighWaterMark(NULL);
  Input1=digitalRead(Button1);
  Input2=digitalRead(Button2);
  Input3=digitalRead(Button3);
  Input4=digitalRead(Button4);
  if (Input1==HIGH && changePage==true){
    Page--;
    selected=false;
    Refresh=true;
    //menuChange=true;
    MainDP=true;
    Serial.println(Page);
    vTaskDelay(500);
  }
  if (Input2==HIGH && changePage==true){
    Page++;
    selected=false;
    Refresh=true;
   // menuChange=true;
    MainDP=true;
    Serial.println(Page);
    vTaskDelay(500);
  }
  if (Page==2 && Input3==HIGH && menuChange==true){
    //mainmenu
     Refresh=true;
     selectedOption = (selectedOption + 1) % menuCount;
     Serial.println(selectedOption);
      Serial.println("main");
    vTaskDelay(200);
  }
  if (Page==2 && Input3==HIGH && selected==true){
    //wifimenu Change
     menuChange=false;
     Refresh=true;
     selectedOption2 = (selectedOption2 + 1) % menu2Count;
     Serial.println(selectedOption2);
     Serial.println("wifi");
    vTaskDelay(200);
  }
  if (Page==2 && Input4==HIGH && menuChange==true){
    //main menu select
    wait = true;
    selected=true;
    MainDP=false;
    Refresh=true;
    menuChange=false;
    Serial.println("selected");
    vTaskDelay(200);
    WifiSubMenu=true;
    WifiSubMenSelected=false;
  }
  if (selectedOption==0 && WifiSubMenu==true && Input4==HIGH && wait==false){
    Refresh=true;
    WifiSubMenSelected=true;
    Serial.println("selectedSub");
    vTaskDelay(600);

  }
  if (selectedOption==3 && menuChange == false && Input4==HIGH && wait==false){
    Refresh=true;
    menuChange=true;
     selected=false; // to return to main menu when selected
      MainDP=true;
    Serial.println("selectedSub");
    vTaskDelay(600);
  }
  
  vTaskDelay(100);
  }
}

void function (void * pvparameters)
{ while (1){
    Page>3?Page=0:Page<0?Page=3:Page=Page;
  vTaskDelay(200);
}

}
void display1 (void *pvparameters)
{ while (1) {
  ramDisplay1 = uxTaskGetStackHighWaterMark(NULL);
  tft.setTextDatum(MC_DATUM);
  if (Page==0){ {
      changePage=false;
      if (Refresh==true){
      tft.fillScreen(TFT_BLACK);
      Serial.println("Ref");
        Refresh=false; }
      if ( Refresh==false && MainDP==true){
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setTextSize(3);
      tft.setCursor(85, 55); //xy
       tft.printf("%04d\n", years);
      tft.setTextSize(2);
      tft.setCursor(90, 85); //xy
      tft.printf("%02d/%02d\n", days, months);
      tft.setTextSize(4);
      tft.setCursor(28, 105); //xy
      tft.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
      tft.setTextSize(2);
      textWidth = tft.textWidth(daysOfWeek[daysOfWeekCount]);
      x = (screenW - textWidth) / 2;
      tft.setCursor(x, 140); //xy
      tft.print(daysOfWeek[daysOfWeekCount]);
      textWidth = tft.textWidth(monthsOfYear[months]);
      x = (screenW - textWidth) / 2;
      tft.setCursor(x, 160); //xy
      tft.print(monthsOfYear[months]);
      changePage=true;
      vTaskDelay(100);
      }
      }
    }
  }
}

void display2 (void *pvparameters)
{ while (1) {
  ramDisplay2 = uxTaskGetStackHighWaterMark(NULL);
  if (Page==1){
    changePage=false;
  if (Refresh==true){
    tft.pushImage(0,0,screenW,screenH,spotify_logo_2_240x240);
    delay(1000);
    tft.fillScreen(TFT_BLACK);
    Serial.println("Ref1");
    Refresh=false; }
  if (Refresh==false && MainDP==true){
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(85, 60); //xy
  tft.printf("Spotify connect");
  vTaskDelay(100);        
  changePage=true;
  }
  }
  vTaskDelay(100);
  }
}


void display3 (void *pvparameters){
  while(1) {
    if (Page==2){
      changePage=false;
      if (Refresh==true){
      tft.fillScreen(TFT_BLACK);
      Refresh=false;
      }
    // main menu
    if (Page==2  && Refresh==false && MainDP==true){
      tft.setTextSize(1);
      int totalMenuWidth = menuColumnCount * (menuItemSize + menuItemSpacing) - menuItemSpacing;
      int totalMenuHeight = menuRowCount * (menuItemSize + menuItemSpacing) - menuItemSpacing;

      int menuStartX = screenW/2 - totalMenuWidth / 2;
      int menuStartY = screenH/2 - totalMenuHeight / 2;

      for (int i = 0; i < menuCount; i++) {
        int row = i / menuColumnCount;
        int col = i % menuColumnCount;

        int x = menuStartX + col * (menuItemSize + menuItemSpacing);
        int y = menuStartY + row * (menuItemSize + menuItemSpacing);

        bool isSelected = (i == selectedOption);

        if (isSelected) {
          // Draw a highlighted rounded rectangle outline around the menu item
          tft.drawRoundRect(x - highlightBorderWidth, y - highlightBorderWidth, menuItemSize + 2 * highlightBorderWidth, menuItemSize + 2 * highlightBorderWidth, menuItemCornerRadius + highlightBorderWidth, TFT_ORANGE);
          tft.setTextColor(TFT_MAROON);
        } else {
          tft.drawRoundRect(x, y, menuItemSize, menuItemSize, menuItemCornerRadius, TFT_WHITE);
          tft.setTextColor(TFT_WHITE);
        }

    // Draw menu item logos and text
      tft.drawString("Option " + String(i + 1), x + 3, y + menuItemSize / 2 - 5);
          }
  //vTaskDelay();
  changePage=true;
       }
       //main menu
        if(selectedOption == 0 && MainDP==false && Refresh==false && WifiSubMenSelected==false){
                menuChange=false;
      //wifi
                tft.setTextSize(1);
                const int menuItemWidth = 180;  // Width of each menu item
                const int menuItemHeight = 30;  // Height of each menu item
                const int menuItemSpacing = 10; // Spacing between menu items

                int totalMenuHeight = menu2Count * (menuItemHeight + menuItemSpacing) - menuItemSpacing;
                int menuStartY = (tft.height() - totalMenuHeight) / 2;

                for (int i = 0; i < menu2Count; i++) {
                  int x = (tft.width() - menuItemWidth) / 2;
                  int y = menuStartY + i * (menuItemHeight + menuItemSpacing);

                  if (i == selectedOption2) {
                    tft.fillRoundRect(x, y, menuItemWidth, menuItemHeight, 10, TFT_YELLOW);
                    tft.setTextColor(TFT_BLACK);
                  } else {
                    tft.drawRoundRect(x, y, menuItemWidth, menuItemHeight, 10, TFT_WHITE);
                    tft.setTextColor(TFT_WHITE);
                  }

                  tft.drawString("Option " + String(i + 1), x + 30, y + 10);
                     // Serial.println("wifiSub");
                }
          delay(100); 
        }
        //wifi
         // wifisub
                if (selectedOption2 == 0 && Refresh==false && WifiSubMenSelected==true ){
                  WifiSubMenu=false;
                  tft.setCursor(120, 120);
                  tft.printf("start");
                  Serial.println("start");
                  delay(1000); 
                }
                if (selectedOption2 == 1  && Refresh==false && WifiSubMenSelected==true){
                  WifiSubMenu=false;
                  tft.setCursor(120, 120);
                  tft.printf("Cancel");
                  delay(1000); 
                }
                if (selectedOption2 == 2  && Refresh==false && WifiSubMenSelected==true){
                  //return to main
                  delay(1000); 
                  WifiSubMenSelected=false;
                  WifiSubMenu=false;
                  menuChange=true;
                  selected=false;
                  MainDP=true;
                }
       // WifiSub
      if(selectedOption == 1 && MainDP==false && Refresh==false){
      //Bluetooth
      menuChange=false;
      tft.setCursor(120, 120);
      tft.printf("Bluetooth");
      delay(1000); 
      }
      if(selectedOption == 2 && MainDP==false && Refresh==false){
     //CLock
     menuChange=false;
     tft.setCursor(120, 120);
      tft.printf("Clock set");
      delay(1000); 
      }
      if(selectedOption == 3 && MainDP==false && Refresh==false){
      //Info

                const char *message[] = {"ESP32-WROOM","ESP32-D0WDQ6","Chip-v4.4.3","ID:DCC84E9EF0C8","Speed-240 Mhz","Flash:4.19 MB","F-Speed:80 Mhz","Flash Mode:0","F-Used:674.06 KB","Cores:2","RAM:256 KB"};
                menuChange=false;
                tft.setTextSize(2);
                textWidth = tft.textWidth(message[0]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 27); //xy
                tft.print(message[0]);

                textWidth = tft.textWidth(message[1]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 45); //xy
                tft.print(message[1]);

                textWidth = tft.textWidth(message[2]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 62); //xy
                tft.print(message[2]);

                textWidth = tft.textWidth(message[3]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 79); //xy
                tft.print(message[3]);

                textWidth = tft.textWidth(message[4]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 95); //xy
                tft.print(message[4]);

                textWidth = tft.textWidth(message[5]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 112); //xy
                tft.print(message[5]);

                textWidth = tft.textWidth(message[6]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 128); //xy
                tft.print(message[6]);

                textWidth = tft.textWidth(message[7]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 145); //xy
                tft.print(message[7]);

                textWidth = tft.textWidth(message[8]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 163); //xy
                tft.print(message[8]);

                textWidth = tft.textWidth(message[9]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 180); //xy
                tft.print(message[9]);
                
                textWidth = tft.textWidth(message[10]);
                x = (screenW - textWidth) / 2;
                tft.setCursor(x, 198); //xy
                tft.print(message[10]);

      delay(1000);
      }
      if(selectedOption == 4 && MainDP==false && Refresh==false){
      //Value
      menuChange=false;
      tft.setCursor(120, 120);
      tft.printf("IMG1");
      delay(1000); 
      }
      if(selectedOption == 5 && MainDP==false && Refresh==false){
      //img2
      menuChange=false;
      tft.setCursor(120, 120);
      tft.printf("IMG2");
      delay(1000); 
      }
    }vTaskDelay(100);
  }
}

void display4 (void *pvparameters){
  while (1) {

    if (Page==3 && Refresh==true){
      tft.fillScreen(TFT_BLACK);
      Refresh=false;
      vTaskDelay(100);
    }
    if (Page==3 && Refresh==false && MainDP==true){
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      const char *message = "Hello, World!";

    int textWidth = tft.textWidth(message);
    int textHeight = tft.fontHeight();

    int x = (tft.width() - textWidth) / 2;
    int y = 10;

    tft.setCursor(x, y);
    tft.println(message);

      vTaskDelay(500);
    }

    vTaskDelay(500);
  }
}
//Battery read
void VoltRead (void * pvparameters) {
  while (1){
  uint32_t Batt_Lvl=0;
   for(int i = 0; i < 16; i++) {
    Batt_Lvl = Batt_Lvl + analogReadMilliVolts(V_Inp); // ADC with correction   
  }
  float Vbattf = 2 * Batt_Lvl / 16 / 1000.0;     // attenuation ratio 1/2, mV --> V
  Serial.println(Vbattf, 3);
  vTaskDelay(3000); // Wait for a second before reading again
}
}
// Timer
void onTimer(TimerHandle_t xTimer) {
    seconds++;
    
    if (seconds > 59) {
        seconds = 0;
        minutes++;
        if (minutes > 59) {
            minutes = 0;
            hours++;
            if (hours > 23) {
                hours = 0;
                days++;
                if (months == 2) {
                        if (years % 4 == 0) {
                            days_Max = 29;
                        } else {
                            days_Max = 28;
                        }
                    } else if (months == 4 || months == 6 || months == 9 || months == 11) {
                        days_Max = 30;
                      } else {
                        days_Max = 31;
                      }
                if (days > days_Max) {
                    days = 1;
                    months++;
                    if (months > 12) {
                        months = 1;
                        years++;
                    }
                    
                }
               
            }
        }
    }
    daysOfWeekCount = (days + (13 * (months + 1)) / 5 + years % 100 + (years % 100) / 4 + (years / 100) / 4 + 5 * (years / 100)) % 7;
    daysOfWeekCount = (daysOfWeekCount + 6) % 7;
  //Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
}


void loop() {
  Serial.println("loop");
  Serial.printf("%02d:%02d:%02d\n", hours, minutes, seconds);
  Serial.print("Memory left for: \n" );
    Serial.printf("-display1: %d \n", ramDisplay1 );
    Serial.printf("-Display2: %d \n", ramDisplay2);
    Serial.printf("-Inp: %d \n", ramInp);
    Serial.printf("-serialMonitor: %d \n", uxTaskGetStackHighWaterMark(NULL)); // to see how many ram left from this task
    Serial.println("--------------------------------------------------");
    Serial.println("-Esp32 Memory left : " + String(ESP.getFreeHeap()) + " bytes"); // to see how many ram left from overall
    Serial.println("--------------------------------------------------");
    Serial.println("-ESP32 Specification" );
    Serial.println("-Version : "+ String(ESP.getSdkVersion()));
    uint64_t chipId = ESP.getEfuseMac(); // Get the chip ID
    Serial.printf("Chip ID: %04X%08X\n", (uint16_t)(chipId >> 32), (uint32_t)chipId);
    Serial.println("-Chip Rev : "+ String(ESP.getChipRevision()));
    Serial.println("-Chip Model : "+ String(ESP.getChipModel()));
    Serial.println("-CPU Freq : "+ String(ESP.getCpuFreqMHz()));
    Serial.println("-Flash Size : "+ String(ESP.getFlashChipSize()));
    Serial.println("-Flash Speed : "+ String(ESP.getFlashChipSpeed()));
    Serial.println("-Flash Mode : "+ String(ESP.getFlashChipMode()));
    Serial.println("-Free Heap : "+ String(ESP.getFreeHeap()));
    Serial.println("-Sketch Size : "+ String(ESP.getSketchSize()));
    Serial.println("-Free Sketch Space : "+ String(ESP.getFreeSketchSpace()));  // flash
    Serial.println("-Cycle Count : "+ String(ESP.getCycleCount()));
    Serial.println("-Cores : "+ String(ESP.getChipCores()));
    Serial.println("-Max heap : "+ String(ESP.getMaxAllocHeap()));
    Serial.println("-Max PSram : "+ String(ESP.getMaxAllocPsram()));    
    Serial.println("--------------------------------------------------");
    Serial.println("--------------------------------------------------");
  delay(1000);
  
  // Add a delay to control the loop execution speed
}

