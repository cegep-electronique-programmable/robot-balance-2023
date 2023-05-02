#ifndef init_h
#define init_h

#include <Adafruit_NeoPixel.h>
#include <StepperNB.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"
#include "interfaces.h"
#include "secrets.h"



int initialisationWiFi(void);
int initialisationI2C(void);
int initialisationSPI(void);
int initialisationBroches(void);
int initialisationSerie(void);
int initialisationsNeoPixel(Adafruit_NeoPixel pixels);
int initialisationOTA(void);

#endif