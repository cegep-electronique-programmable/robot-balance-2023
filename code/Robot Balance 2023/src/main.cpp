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
//  initilisation_reussie += initialisationSerie();
  initilisation_reussie += initialisationBroches();

//  Serial.setPins(GPIO_UART_TX, GPIO_UART_RX);
  Serial.begin(115200);
  printf("Hello, world!\n");

}

void loop() {
  int test = 0;

  while(1){

    printf("%i\r\n", test++);
    
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();    
    delay(1000);              // 1s
    
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();      
    delay(1000);              // 1s
    
  }
  

}