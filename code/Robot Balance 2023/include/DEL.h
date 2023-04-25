#include <Adafruit_NeoPixel.h>
#include "board_mapping.h"

unsigned long pixelPrevious = 0;       // Previous Pixel Millis
unsigned long patternPrevious = 0;     // Previous Pattern Millis
int patternCurrent = 0;                // Current Pattern Number
int patternInterval = 500;             // Pattern Interval (ms)
int pixelInterval = 50;                // Pixel Interval (ms)
int pixelQueue = 0;                    // Pattern Pixel Queue
int pixelCycle = 0;                    // Pattern Pixel Cycle
uint16_t pixelCurrent = 0;             // Pattern Current Pixel Number
uint16_t pixelNumber = NEOPIXEL_COUNT; // Total Number of Pixels


// Instanciation des dels
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;

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
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fonction pour faire un effet de breathing avec les DELs
void breathingEffect(uint32_t color)
{
  for (int i = 0; i < 255; i++)
  {
    pixels.setBrightness(i);
    for (int i = 0; i < NEOPIXEL_COUNT; i++)
    {
        pixels.setPixelColor(i, color);
    }
    pixels.show();
    delay(5);
  }
  for (int i = 255; i > 0; i--)
  {
    pixels.setBrightness(i);
    for (int i = 0; i < NEOPIXEL_COUNT; i++)
    {
        pixels.setPixelColor(i, color);
    }
    pixels.show();
    delay(5);
  }
}

// Fill pixels pixels one after another with a color. pixels is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// pixels.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait)
{
  static unsigned long previousMillis = 0;
  if (millis() - previousMillis >= wait) {
    previousMillis = millis();
    pixels.setPixelColor(pixelCurrent, color); //  Set pixel's color (in RAM)
    pixels.show();                             //  Update pixels to match
    pixelCurrent++;                            //  Advance current pixel
    if (pixelCurrent >= pixelNumber)           //  Loop the pattern from the first LED
      pixelCurrent = 0;
  }
}

// creates a reverse colorWipe
void colorWipeReverse(uint32_t color, int wait)
{
  if (pixelInterval != wait)
    pixelInterval = wait;                    //  Update delay time
  pixels.setPixelColor(pixelCurrent, color); //  Set pixel's color (in RAM)
  pixels.show();                             //  Update pixels to match
  pixelCurrent--;                            //  Advance current pixel
  if (pixelCurrent < 0)                      //  Loop the pattern from the first LED
    pixelCurrent = pixelNumber - 1;
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la pixels.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait)
{
  if (pixelInterval != wait)
    pixelInterval = wait; //  Update delay time
  for (int i = 0; i < pixelNumber; i++)
  {
    pixels.setPixelColor(i + pixelQueue, color); //  Set pixel's color (in RAM)
  }
  pixels.show(); //  Update pixels to match
  for (int i = 0; i < pixelNumber; i += 3)
  {
    pixels.setPixelColor(i + pixelQueue, pixels.Color(0, 0, 0)); //  Set pixel's color (in RAM)
  }
  pixelQueue++; //  Advance current pixel
  if (pixelQueue >= 3)
    pixelQueue = 0; //  Loop the pattern from the first LED
}

// Rainbow cycle along whole pixels. Pass delay time (in ms) between frames.
void rainbow(uint8_t wait)
{
  if (pixelInterval != wait)
    pixelInterval = wait;
  for (uint16_t i = 0; i < pixelNumber; i++)
  {
    pixels.setPixelColor(i, Wheel((i + pixelCycle) & 255)); //  Update delay time
  }
  pixels.show(); //  Update pixels to match
  pixelCycle++;  //  Advance current cycle
  if (pixelCycle >= 256)
    pixelCycle = 0; //  Loop the cycle back to the begining
}

// Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait)
{
  if (pixelInterval != wait)
    pixelInterval = wait; //  Update delay time
  for (int i = 0; i < pixelNumber; i += 3)
  {
    pixels.setPixelColor(i + pixelQueue, Wheel((i + pixelCycle) % 255)); //  Update delay time
  }
  pixels.show();
  for (int i = 0; i < pixelNumber; i += 3)
  {
    pixels.setPixelColor(i + pixelQueue, pixels.Color(0, 0, 0)); //  Update delay time
  }
  pixelQueue++; //  Advance current queue
  pixelCycle++; //  Advance current cycle
  if (pixelQueue >= 3)
    pixelQueue = 0; //  Loop
  if (pixelCycle >= 256)
    pixelCycle = 0; //  Loop
}