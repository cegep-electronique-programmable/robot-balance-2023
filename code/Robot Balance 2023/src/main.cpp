#include <Arduino.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"
#include "interfaces.h"

// ***************  LED  *************** //


#if NEOPIXEL_ACTIVE == 1
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;
#endif


// ***********  PWM MOTEURS  *********** //  
const int PWM_CHANNEL_0 = 0;      //Canal 0 (maximum de 8 canaux)
const int PWM_FREQ_INIT = 200;             //Fréquence PWM
const int PWM_RESOLUTION = 8;        //Résolution PWM (nombre de bits)
const int PWM_DUTY_MAX = 0xFF;        //PWM Duty Cycle max
const int PWM_DUTY_MID = 0x7F;        //PWM Duty Cycle 50%


// ************************************* //

void setup() {
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel(pixels);
  initilisation_reussie += initialisationSerie();
  initilisation_reussie += initialisationBroches();


  // configuration des fonctinonnalités PWM
  ledcSetup(PWM_CHANNEL_0, PWM_FREQ_INIT, PWM_RESOLUTION);
  ledcAttachPin(GPIO_STEP_D, PWM_CHANNEL_0);
  ledcWrite(PWM_CHANNEL_0, PWM_DUTY_MID);

  digitalWrite(GPIO_DIR_D, LOW);
  digitalWrite(GPIO_MS1_D, HIGH);
  digitalWrite(GPIO_MS2_D, HIGH);
  digitalWrite(GPIO_MS3_D, HIGH);


}

void loop() {
  int test = 0;



  while(1){

    printf("%i\r\n", test++);
    
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();    
    ledcChangeFrequency(PWM_CHANNEL_0, PWM_FREQ_INIT, PWM_RESOLUTION);
    delay(1000);              // 500ms
    
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();      
    ledcChangeFrequency(PWM_CHANNEL_0, PWM_FREQ_INIT/8, PWM_RESOLUTION);
    delay(1000);              // 500ms
    
  }
  

}


/*

#include <Arduino.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"
#include "interfaces.h"

// ***************  LED  *************** //

#if NEOPIXEL_ACTIVE == 1
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;
#endif

// ************************************* //

void setup() {
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel(pixels);
  initilisation_reussie += initialisationSerie();
  initilisation_reussie += initialisationBroches();

  // configure LED PWM functionalitites
  ledcSetup(LED_CHANNEL, FREQ, RESOLUTION);
  ledcAttachPin(GPIO_CTRL_MOTOR, LED_CHANNEL);
}

void loop() {
  int test = 0;

  digitalWrite(GPIO_DIR_D, LOW);
  digitalWrite(GPIO_MS1_D, HIGH);
  digitalWrite(GPIO_MS2_D, HIGH);
  digitalWrite(GPIO_MS3_D, HIGH);

  while(1){

    printf("%i\r\n", test++);
    
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();    
    digitalWrite(GPIO_STEP_D, LOW);
    delay(5);              // 500ms
    
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();      
    digitalWrite(GPIO_STEP_D, HIGH);
    delay(5);              // 500ms



    
  }
  

}

*/