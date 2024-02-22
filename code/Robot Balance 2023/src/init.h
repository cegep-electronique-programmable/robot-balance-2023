#ifndef init_h
#define init_h

#include <Adafruit_NeoPixel.h>

#include "config.h"
#include "board_mapping.h"
#include "interfaces.h"

int initialisationBroches(void);
int initialisationSerie(void);
int initialisationsNeoPixel(Adafruit_NeoPixel pixels);

#endif