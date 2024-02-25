#include <Arduino.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"

// ***************  LED  *************** //


#if NEOPIXEL_ACTIVE == 1
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;
#endif


// **********  MAGNETOMETRE  *********** //  

byte angleActuelle = 0;


// ************************************* //

void setup() {
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel(pixels);
  initilisation_reussie += initialisationSerie();
  initilisation_reussie += initialisationBroches();
  initilisation_reussie += initialisationPWMMoteurs();
  initilisation_reussie += initialisationI2C();


}

void loop() {
  int test = 0;
  uint8_t mxc6655xaXout[MXC6655XA_I2C_XOUT_REG_LENGTH];


  while(1){

    printf("%i\r\n", test++);
    
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();    
    //ledcChangeFrequency(PWM_CHANNEL_0, PWM_FREQ_INIT, PWM_RESOLUTION);
    delay(1000);              // 500ms
    
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();      
    //ledcChangeFrequency(PWM_CHANNEL_0, PWM_FREQ_INIT/8, PWM_RESOLUTION);
    delay(1000);              // 500ms



    // Lire l'angle x du magnétomètre MXC6655XA
    
    Wire.beginTransmission(MXC6655XA_I2C_ADDR);
    Wire.write(MXC6655XA_I2C_XOUT_REG); // Register 3: X-axis MSB
    Wire.endTransmission();

    // Request 1 bytes from MXC6655XA
    int nReceived = 0;
    nReceived = Wire.requestFrom(MXC6655XA_I2C_ADDR, MXC6655XA_I2C_XOUT_REG_LENGTH);

    nReceived = Wire.readBytes(mxc6655xaXout, MXC6655XA_I2C_XOUT_REG_LENGTH);

    // Something has gone wrong
    if (nReceived != MXC6655XA_I2C_XOUT_REG_LENGTH)
    {
      printf("Erreur de reception MXC6655XA\r\n");
    }
    else{
      printf("Données reçues: ");
      for (int i = 0; i < MXC6655XA_I2C_XOUT_REG_LENGTH; i++)
      {
        printf("0x%02X ", mxc6655xaXout[i]);
      }
      printf("\r\n");
    
    }
    
  }
  

}


/*
    Wire.beginTransmission(CMPS12_I2C_ADDR);    // Get the slave's attention, tell it we're sending a command byte
    Wire.write(0x01);                           // Envoyer 0x01 au magnétomètre pour dire qu'on veut lire le registre 1, donc le compas
    Wire.endTransmission(false);                // "Hang up the line" so others can use it (can have multiple slaves & masters connected)
    Wire.requestFrom(CMPS12_I2C_ADDR, 2, (int)true);    // Tell slave we need to read 1byte from the current register
    while ( Wire.available() != 0 ){
      printf("%d octets disponibles\r\n", Wire.available());
      angleActuelle = Wire.read();                   // read that byte into 'slaveByte' variable  
      Serial.print("Byte reçu 0x");
      Serial.println(angleActuelle, HEX);
      Serial.print("\n");
    }
*/