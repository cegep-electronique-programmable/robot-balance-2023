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

// Librarie de variables
#include "variables_robot.h"
#include "variables_WIFI.h"

//Fonctions d'initialisation
#include "initialisation_robot.c"
#include "initialisation_WIFI.c"


AsyncWebServer server(80);

void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
}

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
      absolute_position = absolute_position + 16/moteur_gauche.getRatio();
    }
    else {
      absolute_position = absolute_position - 16/moteur_gauche.getRatio();
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

void setup()
{
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel();
  initilisation_reussie += initialisationUART();
  initilisation_reussie += initialisationGPIO();

  // I2C
  Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL);

  delay(500);

  Serial.begin(115200);
  Serial.println("Booting");


  ArduinoOTA.begin();

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

    //printf("SP: %5.2f, Angle: %5.2f, Erreur: %5.2f, Vitesse: %7.2f°/sec\r\n", angle_set_point, angle_average, angle_erreur, vitesse);
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

  
}
