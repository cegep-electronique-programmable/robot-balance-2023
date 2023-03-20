#include <Arduino.h>
#include "board_mapping.h"
#include <Adafruit_NeoPixel.h>

// Instanciation des dels
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Fonction d'initialisation des GPIO
int initialisationGPIO(void) {

  // Boutons utilisateurs (entrées seulement)
  pinMode(GPIO_B1, INPUT);
  pinMode(GPIO_B2, INPUT);

  // Points de test (entrées seulement)
  pinMode(GPIO_TP1, INPUT);
  pinMode(GPIO_TP1, INPUT);

  // Desactivation des moteurs au démarrage
  pinMode(GPIO_ENABLE_MOTEURS, INPUT);
  digitalWrite(GPIO_ENABLE_MOTEURS, LOW);

  // Moteur droit
  pinMode(GPIO_DIR_D, OUTPUT);
  pinMode(GPIO_STEP_D, OUTPUT);
  pinMode(GPIO_MS1_D, OUTPUT);
  pinMode(GPIO_MS2_D, OUTPUT);
  pinMode(GPIO_MS3_D, OUTPUT);
  digitalWrite(GPIO_DIR_D, LOW);
  digitalWrite(GPIO_MS1_D, LOW);
  digitalWrite(GPIO_MS2_D, LOW);
  digitalWrite(GPIO_MS3_D, LOW);

  // Moteur gauche
  pinMode(GPIO_DIR_G, OUTPUT);
  pinMode(GPIO_STEP_G, OUTPUT);
  pinMode(GPIO_MS1_G, OUTPUT);
  pinMode(GPIO_MS2_G, OUTPUT);
  pinMode(GPIO_MS3_G, OUTPUT);
  digitalWrite(GPIO_DIR_G, LOW);
  digitalWrite(GPIO_MS1_G, LOW);
  digitalWrite(GPIO_MS2_G, LOW);
  digitalWrite(GPIO_MS3_G, LOW);

  // LED
  pinMode(GPIO_DATA_LED, OUTPUT);

  // SPI
  pinMode(GPIO_VPSI_CS1, OUTPUT);
  pinMode(GPIO_VPSI_SCK, OUTPUT);
  pinMode(GPIO_VPSI_MISO, INPUT);
  pinMode(GPIO_VPSI_MOSI, OUTPUT);
  digitalWrite(GPIO_VPSI_CS1, HIGH);

  // SPI
  pinMode(GPIO_I2C_SDA, OUTPUT);
  pinMode(GPIO_I2C_SCL, OUTPUT);

  return 0;
}

// Fonction d'initialisation de l'UART
int initialisationUART(void) {
  Serial.begin(115200);
  printf("Robot Balance 2023\r\n");
  return 0;
}

int initialisationsNeoPixel(void) {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(0xFF);
  // Toutes les DEL en rouge pendant l'initialisation
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0xFF, 0x00, 0x00));
  }
  pixels.show();
  return 0;
}

void initialisation_succes(void) {
  // Toutes les DEL en vert après l'initialisation
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0x00, 0xFF, 0x00));
  }
  pixels.show();
  delay(1000);
}

void initialisation_echec(void) {
  // Toutes les DEL en rouge après l'initialisation
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(0xFF, 0x00, 0x00));
  }

  for (int i = 0; i < 20; i++) {
    pixels.show();
    delay(100);
    pixels.clear();
    delay(100);
  }
  
}

void setup() {
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel();
  initilisation_reussie += initialisationUART();
  initilisation_reussie += initialisationGPIO();

  if (initilisation_reussie == 0) {
    initialisation_succes();
  }
  else {
    initialisation_echec();
  }
}

void loop() {
  printf(".\r\n");

  // Défilement des DEL bleues en attendant une connexion sans fil
  int pixel = 0;
  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    if (i == pixel) {
      pixels.setPixelColor(i, pixels.Color(0x00, 0x00, 0xFF)); // Pixel bleu
    }
    else {
      pixels.setPixelColor(i, pixels.Color(0x00, 0x00, 0x00)); // Pixel éteint
    }
    pixels.show();
  }
  pixel = pixel++ % NEOPIXEL_COUNT;

  delay(200);
}