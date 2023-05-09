#include <Arduino.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"
#include "interfaces.h"
#include "secrets.h"

float pitch_accelerometre = 0;
float angle_gyro = 0;
float angle = 0;

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

float getPitchFromCMPS12(void);

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
float pitch_set_point = 5.0;
float output = 0;
float vitesse = 0;

unsigned long previousMillisControlLoop;

float pitch_erreur = 0;
float angulor_velocity = 0;

#define PITCH_KP 50
#define PITCH_KI 0
float dt = 0.0100;
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

// ***************  LOOP  *************** //
void loop()
{
#if OTA_ACTIVE == 1
  ArduinoOTA.handle();
#endif

  // Boucle de controle de la vitesse horizontale
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisControlLoop >= 10)
  {
    previousMillisControlLoop = currentMillis;

    float pitch = getPitchFromCMPS12();
    // printf("%5.2f\r\n", pitch);

    pitch_erreur = pitch_set_point - pitch;
    angulor_velocity = angulor_velocity + pitch_erreur * dt;
    output = PITCH_KP * angle + PITCH_KI * (angulor_velocity);
  
    vitesse = vitesse + output * dt;

  #if MOTORS_ACTIVE == 1
    moteur_gauche.setSpeed(vitesse);
    moteur_droit.setSpeed(vitesse);
  #endif
  }
}

float getPitchFromCMPS12(void)
{
  // PITCHES CMPS12
  float pitch_gyro = 0;

  float pitch = 0;

  int angle_0_255 = 0;

  // Send request to CMPS12
  Wire.beginTransmission(I2C_CMPS12_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  // Request 31 bytes from CMPS12
  int nReceived = 0;
  nReceived = Wire.requestFrom(I2C_CMPS12_ADDRESS, I2C_CMPS12_REGISTER_LENGTH);
  uint8_t data[I2C_CMPS12_REGISTER_LENGTH] = {0};
  nReceived = Wire.readBytes(data, I2C_CMPS12_REGISTER_LENGTH);

  // Something has gone wrong
  if (nReceived != I2C_CMPS12_REGISTER_LENGTH)
  {
    printf("Erreur de reception CMPS12\r\n");
    angle_0_255 = 1;
  }
  else
  {
    // read registers
    angle_0_255 = data[4];
    int16_t gyro_x = (data[0x12] << 8) + data[0x13];
    int16_t gyro_y = (data[0x14] << 8) + data[0x15];
    int16_t gyro_z = (data[0x16] << 8) + data[0x17];
    angulor_velocity = (gyro_x * float(0.015)) / 7.077777777777777777; //((2000.0 / 65535) / 2);

    int16_t acceleration_x = (data[0x0C] << 8) + data[0x0D];
    int16_t acceleration_y = (data[0x0E] << 8) + data[0x0F];
    int16_t acceleration_z = (data[0x10] << 8) + data[0x11];
    pitch_accelerometre = atan2(acceleration_y, acceleration_z) * 180 / PI;

    angle_gyro = angle_gyro + (angulor_velocity * 0.1);
    angle = (0.98) * (angle + angulor_velocity * 0.1) + (0.02) * (pitch_accelerometre);
    // printf("calculated angle : %6f \r", angle); // uyfitf
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillisControlLoop >= 1000)
    {
      previousMillisControlLoop = currentMillis;
      printf("--------------------------------------------------------------------\n");
      printf("angulor velocity : %6f \n", angulor_velocity);
      printf("acceleration_pitch : %6f \n", pitch_accelerometre);
      printf("gyroscope angle : %6f \n", angle_gyro);
      printf("angle : %6f \n", angle);
    }
  }
  // return pitch in degrees
  return pitch;
}
