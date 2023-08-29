#include <Arduino.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"
#include "interfaces.h"
#include "secrets.h"

// ***************  WIFI  *************** //
#if WIFI_ACTIVE == 1
#include <WiFi.h>
#include <WiFiUdp.h>
#endif

#if EAP_ACTIVE == 1
#include "esp_wpa2.h"
#endif

#if OTA_ACTIVE == 1
#include <ArduinoOTA.h>
#include "esp_wpa2.h"
#endif

#if WEBSERIAL_ACTIVE == 1
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#endif

// ***************  LED  *************** //

#if NEOPIXEL_ACTIVE == 1
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;
#endif

// ***************  I2C  *************** //
#if I2C_ACTIVE == 1
#include <Wire.h>
#endif



void getDataFromCMPS12(uint8_t *data);
uint8_t data[I2C_CMPS12_REGISTER_LENGTH] = {0};

float getTiltFromCMPS12(void);
float getAngularSpeedFromCMPS12(void);

unsigned long gyroscope_previous_micro = 0;
unsigned long gyroscope_current_micro = 0;
float angle_from_gyro = 0;
float angle_from_cmps12 = 0;

// ***************  SPI  *************** //

#if SPI_ACTIVE == 1
#include <SPI.h>
#endif

#if SPI_ACTIVE == 1 && MXC6655_ACTIVE == 1
#include "MXC6655.h"
MXC6655 accel;
#endif

// ***************  MOTEURS  *************** //

#if MOTORS_ACTIVE == 1
#include <StepperNB.h>
StepperNB moteur_gauche(GPIO_DIR_G, GPIO_STEP_G, GPIO_MS1_G, GPIO_MS2_G, GPIO_MS3_G, 200, false);
StepperNB moteur_droit(GPIO_DIR_D, GPIO_STEP_D, GPIO_MS1_D, GPIO_MS2_D, GPIO_MS3_D, 200, true);
hw_timer_t *Timer0_Cfg = NULL; // Moteur Gauche
hw_timer_t *Timer3_Cfg = NULL; // Moteur Droit
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Fonction d'interruption pour envoyer les impulsions au moteur gauche
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
  }

  timerAlarmWrite(Timer0_Cfg, delay, true);
  timerAlarmEnable(Timer0_Cfg);

  interrupts();
  portEXIT_CRITICAL(&timerMux);
}

// Fonction d'interruption pour envoyer les impulsions au moteur droit
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
#endif



// ***************  CONTROL  *************** //
float tilt_set_point = -0.2;
float tilt_speed_set_point = 0;

float tilt_erreur = 0;
float tilt_erreur_somme = 0;
float tilt_speed_erreur = 0;

float vitesse = 0;

unsigned long previousMillisControlLoop;

#define DIAMETRE_ROUE 0.91

#define KP -13
#define KI -1
#define KD -1

float dt = 0.001;
/********************************************/




// ***************  SETUP  *************** //
void setup()
{
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel(pixels);
  initilisation_reussie += initialisationSerie();
  initilisation_reussie += initialisationBroches();
  initilisation_reussie += initialisationI2C();
  initilisation_reussie += initialisationSPI();

#if MOTORS_ACTIVE == 1
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

  // Enable motors
  pinMode(GPIO_ENABLE_MOTEURS, OUTPUT);
  digitalWrite(GPIO_ENABLE_MOTEURS, LOW);
#else
  // Disable motors
  digitalWrite(GPIO_ENABLE_MOTEURS, HIGH);
#endif
}

uint8_t tilt_index = 0;
float tilt_array[10];


// ***************  LOOP  *************** //
void loop()
{
#if OTA_ACTIVE == 1
  ArduinoOTA.handle();
#endif

  // lecture des capteurs
  getDataFromCMPS12(data);

  float tilt = getTiltFromCMPS12();
  printf("Tilt: %f\r\n", tilt);

  /*tilt_array[tilt_index] = tilt;

  tilt_index++;

  if (tilt_index >= 10)
  {
    tilt_index = 0;
  }

  tilt = 0;
  for (int i = 0; i < 10; i++)
  {
    tilt += tilt_array[i];
  }
  tilt = tilt / 10;
  */

  if ((tilt > 25.0) || (tilt < -25.0))
  {
    // Disable motors
    digitalWrite(GPIO_ENABLE_MOTEURS, HIGH);
  }
  else
  {
    // Enable motors
    digitalWrite(GPIO_ENABLE_MOTEURS, LOW);
  }

  
  //getTiltFromCMPS12();

  // Boucle de controle de la vitesse horizontale
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisControlLoop >= 100)
  {
    previousMillisControlLoop = currentMillis;

    tilt_erreur = tilt_set_point - getTiltFromCMPS12();
    tilt_speed_erreur = tilt_speed_set_point - getAngularSpeedFromCMPS12();
    tilt_erreur_somme = tilt_erreur_somme + tilt_erreur * dt;
    float acceleration = KP * tilt_erreur + KI * tilt_erreur_somme + KD * tilt_speed_erreur;
    #define ACCELERATION_MAX 150
    acceleration = acceleration > ACCELERATION_MAX ? ACCELERATION_MAX : acceleration;
    acceleration = acceleration < -ACCELERATION_MAX ? -ACCELERATION_MAX : acceleration;
    vitesse = moteur_droit.getSpeed() + acceleration;
    #define VITESSE_MAX 720
    vitesse = vitesse > VITESSE_MAX ? VITESSE_MAX : vitesse;
    vitesse = vitesse < -VITESSE_MAX ? -VITESSE_MAX : vitesse;
  }

#if MOTORS_ACTIVE == 1
  moteur_gauche.setSpeed(vitesse);
  moteur_droit.setSpeed(vitesse);
#endif
}



void getDataFromCMPS12(uint8_t *data)
{
  // Send request to CMPS12
  Wire.beginTransmission(I2C_CMPS12_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  // Request 31 bytes from CMPS12
  int nReceived = 0;
  nReceived = Wire.requestFrom(I2C_CMPS12_ADDRESS, I2C_CMPS12_REGISTER_LENGTH);

  nReceived = Wire.readBytes(data, I2C_CMPS12_REGISTER_LENGTH);

  // Something has gone wrong
  if (nReceived != I2C_CMPS12_REGISTER_LENGTH)
  {
    printf("Erreur de reception CMPS12\r\n");
  }
}

float getTiltFromCMPS12(void)
{
  // Calcul l'angle à partir de l'accelerometre
  //int16_t accelerometer_x = data[0x0C] << 8 | data[0x0D];
  int16_t accelerometer_y = data[0x0E] << 8 | data[0x0F];
  int16_t accelerometer_z = data[0x10] << 8 | data[0x11];
  float angle_from_accelerometer = atan2f(accelerometer_y, accelerometer_z) * 180.0 / PI;

  // Calcul l'angle à partir du gyroscope
  gyroscope_current_micro = micros();
  float gyroscope_dt = (gyroscope_current_micro - gyroscope_previous_micro) / 1000000.0;
  int16_t gyroscope_x = (data[0x12] << 8) | data[0x13];
  float angulare_speed_from_gyro = (float)gyroscope_x * 2000.0 / (float)0x7FFF;
  gyroscope_previous_micro = gyroscope_current_micro;

  // Angle filtré
  // https://web.mit.edu/first/segway/#misc > https://web.mit.edu/first/segway/segspecs.zip > filter.pdf
  float ratio = 0.02;
  angle_from_cmps12 = (1 - ratio) * (angle_from_cmps12 + angulare_speed_from_gyro * gyroscope_dt) + (ratio) * (angle_from_accelerometer);

  return angle_from_cmps12;
}

float getAngularSpeedFromCMPS12(void) {
  int16_t gyroscope_x = (data[0x12] << 8) | data[0x13];
  float angulare_speed_from_gyro = (float)gyroscope_x * 2000.0 / (float)0x7FFF;
  return angulare_speed_from_gyro;
}