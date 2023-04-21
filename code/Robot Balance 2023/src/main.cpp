#include <Arduino.h>
#include "board_mapping.h"
#include "interfaces.h"

#include <Wire.h>
#include <SPI.h>
#include "MXC6655.h"
#include <Adafruit_NeoPixel.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#include <StepperNB.h>


unsigned long pixelPrevious = 0;        // Previous Pixel Millis
unsigned long patternPrevious = 0;      // Previous Pattern Millis
int           patternCurrent = 0;       // Current Pattern Number
int           patternInterval = 500;   // Pattern Interval (ms)
int           pixelInterval = 50;       // Pixel Interval (ms)
int           pixelQueue = 0;           // Pattern Pixel Queue
int           pixelCycle = 0;           // Pattern Pixel Cycle
uint16_t      pixelCurrent = 0;         // Pattern Current Pixel Number
uint16_t      pixelNumber = NEOPIXEL_COUNT;  // Total Number of Pixels


AsyncWebServer server(80);

void recvMsg(uint8_t *data, size_t len)
{
  WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }
  WebSerial.println(d);
}

StepperNB moteur_gauche(GPIO_DIR_G, GPIO_STEP_G, GPIO_MS1_G, GPIO_MS2_G, GPIO_MS3_G, 200, false);
StepperNB moteur_droit(GPIO_DIR_D, GPIO_STEP_D, GPIO_MS1_D, GPIO_MS2_D, GPIO_MS3_D, 200, true);

float target_speed_degrees_per_second_moteur_1 = 90;
int ratio_moteur_1 = 4;
int step_per_tour_moteur_1 = 200;
int delai_timer_moteur_1 = 1000000;

float target_speed_degrees_per_second_moteur_2 = 45;
int ratio_moteur_2 = 4;
int step_per_tour_moteur_2 = 200;
int delai_timer_moteur_2 = 1000000;

int angle_index = 0;
int angle_samples[10];
float angle_average = 0;

unsigned long previousMillisControlLoop1;
unsigned long previousMillisControlLoop2;

float angle_set_point = 5;
int angle = 0;
float vitesse = 0;

float absolute_position_set_point = 0;
int32_t absolute_position = 0;
float absolute_position_error = 0;

float angle_erreur = 0;
float angle_erreur_somme = 0;

#define KP_SPEED 0.001
#define DIAMETRE_ROUE 0.91

#define KP 75
#define KI 100
float dt = 0.02;

#define WIFI_ACTIVE ENTERPRISE

#include "secrets.h"

#ifdef WIFI_ACTIVE
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#if WIFI_ACTIVE != ENTERPRISE
const char *host = WIFI_HOST_NAME;
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
#else
#include "esp_wpa2.h"
const char *ssid = EAP_SSID;
int counter = 0;
#endif
#endif

hw_timer_t *Timer0_Cfg = NULL; // Moteur Gauche
hw_timer_t *Timer3_Cfg = NULL; // Moteur Droit

int angle_sp = 8;
int erreur = 0;
int output = 0;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR Timer0_MoteurG_ISR()
{
  portENTER_CRITICAL(&timerMux);
  noInterrupts();

  uint64_t delay = moteur_gauche.getTimerPeriod();
  if (delay > 100000)
  {
    delay = 100000;
  }
  delay = delay > 100000 ? 100000 : delay;
  if (delay < 100000)
  {
    digitalWrite(GPIO_STEP_G, HIGH);
    delayMicroseconds(2);
    digitalWrite(GPIO_STEP_G, LOW);
    if (moteur_gauche.getDirection() == 0)
    {
      absolute_position = absolute_position + 16 / moteur_gauche.getRatio();
    }
    else
    {
      absolute_position = absolute_position - 16 / moteur_gauche.getRatio();
    }
  }

  timerAlarmWrite(Timer0_Cfg, delay, true);
  timerAlarmEnable(Timer0_Cfg);

  interrupts();
  portEXIT_CRITICAL(&timerMux);
}

void IRAM_ATTR Timer3_MoteurD_ISR()
{
  portENTER_CRITICAL(&timerMux);
  noInterrupts();

  uint64_t delay = moteur_droit.getTimerPeriod();
  if (delay > 100000)
  {
    delay = 100000;
  }
  if (delay < 100000)
  {
    digitalWrite(GPIO_STEP_D, HIGH);
    delayMicroseconds(2);
    digitalWrite(GPIO_STEP_D, LOW);
  }

  timerAlarmWrite(Timer3_Cfg, delay, true);
  timerAlarmEnable(Timer3_Cfg);

  interrupts();
  portEXIT_CRITICAL(&timerMux);
}

// Accéléromètre
MXC6655 accel;

// Instanciation des dels
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;

int timeoutVAR = 0;

// Fonction d'initialisation des GPIO
int initialisationGPIO(void)
{

  // Boutons utilisateurs (entrées seulement)
  pinMode(GPIO_B1, INPUT);
  pinMode(GPIO_B2, INPUT);

  // Points de test (entrées seulement)
  pinMode(GPIO_TP1, INPUT);
  pinMode(GPIO_TP1, INPUT);

  // Desactivation des moteurs au démarrage
  pinMode(GPIO_ENABLE_MOTEURS, INPUT);
  digitalWrite(GPIO_ENABLE_MOTEURS, LOW);

  // Moteur droit
  pinMode(GPIO_DIR_D, OUTPUT);
  pinMode(GPIO_STEP_D, OUTPUT);
  pinMode(GPIO_MS1_D, OUTPUT);
  pinMode(GPIO_MS2_D, OUTPUT);
  pinMode(GPIO_MS3_D, OUTPUT);
  digitalWrite(GPIO_DIR_D, LOW);
  digitalWrite(GPIO_MS1_D, LOW);
  digitalWrite(GPIO_MS2_D, LOW);
  digitalWrite(GPIO_MS3_D, LOW);

  // Moteur gauche
  pinMode(GPIO_DIR_G, OUTPUT);
  pinMode(GPIO_STEP_G, OUTPUT);
  pinMode(GPIO_MS1_G, OUTPUT);
  pinMode(GPIO_MS2_G, OUTPUT);
  pinMode(GPIO_MS3_G, OUTPUT);
  digitalWrite(GPIO_DIR_G, LOW);
  digitalWrite(GPIO_MS1_G, LOW);
  digitalWrite(GPIO_MS2_G, LOW);
  digitalWrite(GPIO_MS3_G, LOW);

  // LED
  pinMode(GPIO_DATA_LED, OUTPUT);

  // SPI
  pinMode(GPIO_VPSI_CS1, OUTPUT);
  pinMode(GPIO_VPSI_SCK, OUTPUT);
  pinMode(GPIO_VPSI_MISO, INPUT);
  pinMode(GPIO_VPSI_MOSI, OUTPUT);
  digitalWrite(GPIO_VPSI_CS1, HIGH);

  // SPI
  pinMode(GPIO_I2C_SDA, OUTPUT);
  pinMode(GPIO_I2C_SCL, OUTPUT);

  return 0;
}

// Fonction d'initialisation de l'UART
int initialisationUART(void)
{
  Serial.setPins(GPIO_UART_TX, GPIO_UART_RX);
  Serial.begin(115200);
  printf("Robot Balance 2023\r\n");
  return 0;
}

int initialisationsNeoPixel(void)
{
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(0x22);
  // Toutes les DEL en rouge pendant l'initialisation
  for (int i = 0; i < NEOPIXEL_COUNT; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0x00, 0x00, 0x00));
  }
  pixels.show();
  return 0;
}

// Some functions of our own for creating animated effects -----------------

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill pixels pixels one after another with a color. pixels is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// pixels.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time
  pixels.setPixelColor(pixelCurrent, color); //  Set pixel's color (in RAM)
  pixels.show();                             //  Update pixels to match
  pixelCurrent++;                           //  Advance current pixel
  if(pixelCurrent >= pixelNumber)           //  Loop the pattern from the first LED
    pixelCurrent = 0;
}

// creates a reverse colorWipe
void colorWipeReverse(uint32_t color, int wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time
  pixels.setPixelColor(pixelCurrent, color); //  Set pixel's color (in RAM)
  pixels.show();                             //  Update pixels to match
  pixelCurrent--;                           //  Advance current pixel
  if(pixelCurrent < 0)                      //  Loop the pattern from the first LED
    pixelCurrent = pixelNumber - 1;
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la pixels.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time
  for(int i = 0; i < pixelNumber; i++) {
    pixels.setPixelColor(i + pixelQueue, color); //  Set pixel's color (in RAM)
  }
  pixels.show();                             //  Update pixels to match
  for(int i=0; i < pixelNumber; i+=3) {
    pixels.setPixelColor(i + pixelQueue, pixels.Color(0, 0, 0)); //  Set pixel's color (in RAM)
  }
  pixelQueue++;                             //  Advance current pixel
  if(pixelQueue >= 3)
    pixelQueue = 0;                         //  Loop the pattern from the first LED
}

// Rainbow cycle along whole pixels. Pass delay time (in ms) between frames.
void rainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   
  for(uint16_t i=0; i < pixelNumber; i++) {
    pixels.setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time  
  }
  pixels.show();                             //  Update pixels to match
  pixelCycle++;                             //  Advance current cycle
  if(pixelCycle >= 256)
    pixelCycle = 0;                         //  Loop the cycle back to the begining
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  if(pixelInterval != wait)
    pixelInterval = wait;                   //  Update delay time  
  for(int i=0; i < pixelNumber; i+=3) {
    pixels.setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time  
  }
  pixels.show();
  for(int i=0; i < pixelNumber; i+=3) {
    pixels.setPixelColor(i + pixelQueue, pixels.Color(0, 0, 0)); //  Update delay time  
  }      
  pixelQueue++;                           //  Advance current queue  
  pixelCycle++;                           //  Advance current cycle
  if(pixelQueue >= 3)
    pixelQueue = 0;                       //  Loop
  if(pixelCycle >= 256)
    pixelCycle = 0;                       //  Loop
}



void setup()
{
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel();
  initilisation_reussie += initialisationUART();
  initilisation_reussie += initialisationGPIO();

  // I2C
  //Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL);

  delay(500);

  Serial.begin(115200);
  Serial.println("Booting");

#ifdef WIFI_ACTIVE
  WiFi.mode(WIFI_STA);
#if WIFI_ACTIVE != ENTERPRISE
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
#else
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;
    if (counter >= 60)
    { // after 30 seconds timeout - reset board
      ESP.restart();
    }
  }
#endif

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();

#endif

  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();

  // I2C
  Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL);

  moteur_gauche.setSpeed(0);
  moteur_droit.setSpeed(0);

  moteur_gauche.setRatio(16);
  moteur_droit.setRatio(16);

  Timer0_Cfg = timerBegin(0, 80, true); // timer incrémente toutes les 1us
  timerAttachInterrupt(Timer0_Cfg, &Timer0_MoteurG_ISR, true);
  timerAlarmWrite(Timer0_Cfg, 1000000, true); // delai d'une seconde au démarrage
  timerAlarmEnable(Timer0_Cfg);

  Timer3_Cfg = timerBegin(3, 80, true); // timer incrémente toutes les 1us
  timerAttachInterrupt(Timer3_Cfg, &Timer3_MoteurD_ISR, true);
  timerAlarmWrite(Timer3_Cfg, 1000000, true); // delai d'une seconde au démarrage
  timerAlarmEnable(Timer3_Cfg);

  pinMode(GPIO_ENABLE_MOTEURS, OUTPUT);
  digitalWrite(GPIO_ENABLE_MOTEURS, LOW);

  SPI.begin(GPIO_VPSI_SCK, GPIO_VPSI_MISO, GPIO_VPSI_MOSI, GPIO_VPSI_CS1);

}

void loop()
{
  ArduinoOTA.handle();
   // Fill along the length of the pixels in various colors...
  //colorWipe(pixels.Color(255,   0,   0)     , 50); // Red
  //colorWipe(pixels.Color(  0, 255,   0)     , 50); // Green
  //colorWipe(pixels.Color(  0,   0, 255)     , 50); // Blue
  //colorWipe(pixels.Color(  0,   0,   0, 255), 50); // True white (not RGB white)

   unsigned long currentMillis = millis();                     //  Update current time
  if((currentMillis - patternPrevious) >= patternInterval) {  //  Check for expired time
    patternPrevious = currentMillis;
    patternCurrent++;                                         //  Advance to next pattern
    if(patternCurrent >= 6)
      patternCurrent = 0;

    WebSerial.print("Allo");
  }
  
  if(currentMillis - pixelPrevious >= pixelInterval) {        //  Check for expired time
    pixelPrevious = currentMillis;                            //  Run current frame
    switch (patternCurrent) {
      case 7:
        break;
      case 6:
        break;     
      case 5:
        break;
      case 4:
        colorWipeReverse(pixels.Color(0, 0, 0), 50); // Red
        break;
      case 3:
        colorWipeReverse(pixels.Color(0, 0, 255), 50); // Blue
        break;
      case 2:
        break;
      case 1:
        colorWipe(pixels.Color(0, 0, 0), 50); // Green
        break;        
      default:
        colorWipe(pixels.Color(255, 0, 0), 50); // Red
        break;
    }
  }

  //pulseWhite(5);

  //rainbowFade2White(3, 3, 1);
  /*
  

  int nReceived = 0;
  int angle_0_255 = 0;

  Wire.beginTransmission(I2C_CMPS12_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  // Request 31 bytes from CMPS12
  nReceived = Wire.requestFrom(I2C_CMPS12_ADDRESS, I2C_CMPS12_REGISTER_LENGTH);
  uint8_t data[I2C_CMPS12_REGISTER_LENGTH] = {0};

  nReceived = Wire.readBytes(data, I2C_CMPS12_REGISTER_LENGTH);
  // Something has gone wrong
  if (nReceived != I2C_CMPS12_REGISTER_LENGTH)
  {
    printf("Erreur de reception\r\n");
    angle_0_255 = 1;
  }
  else
  {
    angle_0_255 = data[4];
  }

  if (angle_0_255 > 127)
  {
    angle = angle_0_255 - 256;
  }
  else
  {
    angle = angle_0_255;
  }

  accel.begin();
  float accX = accel.getAccel(0, 0);
  float accY = accel.getAccel(1, 0);
  float accZ = accel.getAccel(2, 0);
  float temp = accel.getTemp();
  float theta = atan2(accX, accZ) * 180 / PI;

// Moving average on angle over 10 samples
#define RATIO 0.95
  angle_samples[angle_index] = (0.8 * (float)angle) + (float)(1 - RATIO) * theta;
  angle_index = angle_index + 1;
  if (angle_index >= 10)
  {
    angle_index = 0;
  }

  angle_average = 0;
  for (int i = 0; i < 10; i++)
  {
    angle_average = (float)angle_average + (float)angle_samples[i];
  }
  angle_average = (float)angle_average * 0.1;

  // Boucle de controle de la vitesse horizontale
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisControlLoop2 >= 200)
  {
    previousMillisControlLoop2 = currentMillis;

    absolute_position_set_point = 0;
    absolute_position_error = absolute_position_set_point - absolute_position;
    angle_set_point = KP_SPEED * absolute_position_error + 5;

    angle_set_point = angle_set_point > 10 ? 10 : angle_set_point;
    angle_set_point = angle_set_point < 0 ? 0 : angle_set_point;

    // printf("SP: %5.2f, Angle: %5.2f, Erreur: %5.2f, Vitesse: %7.2f°/sec\r\n", angle_set_point, angle_average, angle_erreur, vitesse);
    WebSerial.print("Angle: ");
    WebSerial.println(angle_average);
  }

  // Boucle de controle de l'inclinaison
  currentMillis = millis();

  if (currentMillis - previousMillisControlLoop1 >= 20)
  {
    previousMillisControlLoop1 = currentMillis;

    angle_erreur = angle_set_point - angle_average;
    angle_erreur_somme = angle_erreur_somme + angle_erreur * dt;
    vitesse = KP * angle_erreur + KI * (angle_erreur_somme);

    absolute_position = PI * DIAMETRE_ROUE * vitesse / 360;
  }

  moteur_gauche.setSpeed(vitesse);
  moteur_droit.setSpeed(vitesse);

  // digitalWrite(GPIO_ENABLE_MOTEURS, HIGH);

  //
  // printf("Accelerations X: %5.2f, Z: %5.2f, Theta: %5.2f, Temps: %5.2f\r\n", accX, accZ, theta, temp);
  */
}

