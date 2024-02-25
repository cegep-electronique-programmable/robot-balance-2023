#ifndef init_h
#define init_h

#include <Adafruit_NeoPixel.h>

#include "config.h"
#include "board_mapping.h"
#include <Wire.h>

// ***********  PWM MOTEURS  *********** //  

const int PWM_CHANNEL_0 = 0;      //Canal 0 (maximum de 8 canaux)
const int PWM_FREQ_INIT = 0;             //Fréquence PWM
const int PWM_RESOLUTION = 8;        //Résolution PWM (nombre de bits)
const int PWM_DUTY_MAX = 0xFF;        //PWM Duty Cycle max
const int PWM_DUTY_MID = 0x7F;        //PWM Duty Cycle 50%


// **********  MAGNETOMETRE  *********** //  
const int CMPS12_I2C_ADDR = 0x60; //0xC0 >> 1bit puisque le bit 0 est pour write
const int CMPS12_I2C_REGISTER_LENGTH = 0x1E;
const int CMPS12_I2C_BAUD_RATE = 9600;

const int MXC6655XA_I2C_ADDR = 0x15;
const int MXC6655XA_I2C_XOUT_REG = 0x03;
const int MXC6655XA_I2C_XOUT_REG_LENGTH = 2;
const int MXC6655XA_I2C_REGISTER_LENGTH = 0x0F;
const int MXC6655XA_I2C_BAUD_RATE = 115200;

// ************************************* //

int initialisationBroches(void);
int initialisationSerie(void);
int initialisationsNeoPixel(Adafruit_NeoPixel pixels);
int initialisationPWMMoteurs(void);
int initialisationI2C(void);

#endif