# Robot Balance 2023

Informations techniques du robot qui tient en équilibre sur deux roues.

## Présentation

Le **Robot Balance 2023** est une évolution du robot 2017. Les objectifs de la nouvelle version sont :
* Enlever les fils d'alimentation : Le robot est maintenant alimenté par une **batterie 12V**.
* Surélever le centre de gravité pour **améliorer le contrôle**. La batterie est située en haut du robot.
* Pouvoir commander et observer le robot sans fils : La carte LPC1768 est remplacée par une carte à base de **ESP32 (Wifi & Bluetooth)**.

## Aperçu du développement

### Mécanique 

La mécanique est développée dans Fusion 360 et elle est basée sur un cadre en extrusions d'aluminium.
Les roues sont entrainées par des moteurs pas-à-pas de type NEMA-17.

### Électronique

La carte électronique est développée dans Fusion 360 et elle est basée sur :
* Un microcontrôleur ESP32-WROOM-32.
* Deux commandes moteur Polulu A4988.
* Un capteur ADXL.
* Un capteur CMPS12.
* Des régulateurs d'alimentation AP63203 et AP2112K.

### Firmware

Le firmware est développé à l'aide de l'outil PlatformIO. Nous utilisons le framework (TBD: Arduino ou IDK).

Le contrôle se fait à l'aide d'un contrôleur de type Proportionnel-Intégral-Dérivé (PID).

## Description détaillée

### Mécanique 

* [ ] Section à compléter

### Électronique

![render](https://github.com/cegep-electronique-programmable/robot-balance-2023/blob/main/Media/render%20v2.png?raw=true)

#### Composants principaux

La carte est conçue avec les composants principaux suivants :

- 1 x Microcontrôleur ESP32-WROOM
- 2 x Driver de stepper motor Polulu A4988
- 1 x Accéleromètre, inclinomètre et magnétomètre combiné CMPS12
- 1 x Accéleromètre TBD
- 2 x Boutons utilisateurs
- 5 x DEL utilisateur RGB en série
- 2 x Connecteur I2C
- 1 x Connecteur SPI

#### Choix des broches du microcontrôleur

| # Pin | GPIO | TYPE | Mode | Signal | Commentaire |
| --- | --- | --- | --- | --- | --- |
| Pin 1 | N/A | P |  Alimentation | `GND` |
| Pin 2 | N/A |P|  Alimentation | `3V3` |
| Pin 3 | N/A |I|  Contrôle | `RESET` | Enable |
| Pin 4 | GPIO 36 |I|  Bouton 1 | `B1` |
| Pin 5 | GPIO 39 |I|  Bouton 2 | `B2` |
| Pin 6 | GPIO 34 |I| TP1 |  `TP1` |
| Pin 7 | GPIO 35 |I| TP2 | `TP2` |
| Pin 8 | GPIO 32 |I/O|  Enables des moteurs  | `EN_MOTEURS` |
| Pin 9 | GPIO 33 |I/O|  Contôle du moteur droit  | `DIR_D` |
| Pin 10 | GPIO 25 |I/O|  Contôle du moteur droit  | `STEP_D` |
| Pin 11 | GPIO 26 |I/O|  Contôle du moteur droit  | `MS3_D` |
| Pin 12 | GPIO 27 |I/O|  Contôle du moteur droit  | `MS2_D` |
| Pin 13 | GPIO 14 |I/O|  Contôle du moteur droit  | `MS1_D` |  |  |
| Pin 14 | GPIO 12 |I/O (Strap) |  Réservée |  | Configuration de la tension d'alimentation |
| Pin 15 | N/A |P|  Alimentation | `GND` |
| Pin 16 | GPIO 13 |I/O|  Contôle du moteur gauche | `MS3_G` |
| Pin 17 | GPIO 9 |-|  Flash interne |  | Ne pas réutiliser |
| Pin 18 | GPIO 10 |-|  Flash interne |  | Ne pas réutiliser |
| Pin 19 | GPIO 11 |-|  Flash interne |  | Ne pas réutiliser |
| Pin 20 | GPIO 6 |-|  Flash interne |  | Ne pas réutiliser |
| Pin 21 | GPIO 7 |-|  Flash interne |  | Ne pas réutiliser |
| Pin 22 | GPIO 8 |-|  Flash interne |  | Ne pas réutiliser |
| Pin 23 | GPIO 15 |I/O (Strap) |  Contôle du moteur gauche  | `MS2_G` |
| Pin 24 | GPIO 2 |I/O (Strap) |   Contôle du moteur gauche  | `MS1_G` |
| Pin 25 | GPIO 0 |I/O (Strap) |  Démarrage | `BOOT` | Boot SPI ou Upload |
| Pin 26 | GPIO 4 |I/O|  data neopixel | `DATA_LED` | Sortie pour bus NeoPixels |
| Pin 27 | GPIO 16 |I/O|  Contôle du moteur gauche  | `STEP_G` |
| Pin 28 | GPIO 17 |I/O|  Contôle du moteur gauche| `DIR_G` |
| Pin 29 | GPIO 5 |I/O (Strap) |  VSPI | `CS1` | Chip Select 1 SPI |
| Pin 30 | GPIO 18 |I/O|  VSPI | `SCK` | Horloge SPI |
| Pin 31 | GPIO 19 |I/O|  VSPI | `MISO` | Entrée SPI |
| Pin 32 | N/A |-|  NC | Aucun | Non connectée |
| Pin 33 | GPIO 21 |I/O|  I2C | `I2C_SDA` | Signal de données I2C |
| Pin 34 | GPIO 3 |I/O|  UART | `RXD0` | Broche de reception |
| Pin 35 | GPIO 1 |I/O|  UART | `TXD0` | Broche d'émission |
| Pin 36 | GPIO 22 |I/O|  I2C | `I2C_SCL` | Signal d'horloge I2C |
| Pin 37 | GPIO 23 |I/O|  VSPI | `MOSI` | Sortie SPI |
| Pin 38 | N/A |P|  Alimentation | `GND` |
| Pin 39 | N/A |THERMAL PAD|  Alimentation | `GND` |




#### SPI 🟠

SPI 0 et 1 sont utilisés à l'interne du module.

On peut utiliser SPI2 (HSPI) ou SPI3 (VSPI). J'ai choisi HSPI pour concerver le I2C sur les broches 36, 37.

[ESP32 Manuel de Référence Technique - Section 7](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#spi)

Les infos sur le shcéma adafruit placent les sorties MOSI à la pin 30 (GPIO 18), MISO à la pin 31 (GPIO 19) et SCK à la pin 29 (GPIO 5)



#### Guide pour le PCB Layout

PCB à 4 couches :
| Couche | Signaux |
| -- | -- |
| 1 | Traces et composants |
| 2 | Plan de ground `GND` |
| 3 | Alimentations |
| 4 | Traces de signaux et d'alimentations (pas de composant si possible) |

Garder 15 mm d'espaces sur les 3 côtés de l'antenne.


### Microgiciel (Firmware)

#### Numéros de broches

Les numéros de broches sont configurés dans le fichier `board_mapping.h`.


#### UART

Pensez à assigner les bons numéros de broches :

```
Serial.setPins(GPIO_UART_TX, GPIO_UART_RX);
Serial.begin(115200);
```

Dans le fichier `platformio.ini`, utiliser la configuration suivante pour empêcher un `Reset` à l'ouverture du terminal :
```
monitor_rts = 0
monitor_dtr = 0
```


#### I2C

Pensez à assigner les bons numéros de broches :

```
Wire.begin(GPIO_I2C_SDA, GPIO_I2C_SCL);
```

##### Adresses par défaut

| Composant | Adresse | 
| --- | --- | 
| CMPS12 | 0x60 |
| MCX6655 | 0x15 |

# Références : 
* [ESP32 DevKitC v4-sch](https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf)
* [ESP32 Adafruit Huzzah32](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather)
* [HDG - Hardware Design Guideline](https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_en.pdf)
* [ESP32_WROOM datasheet](https://www.mouser.com/datasheet/2/891/esp-wroom-32_datasheet_en-1223836.pdf)
* [Blog: ESP32 Pinout simplified: No more confusion when choosing GPIOs.](https://www.techtonions.com/esp32-pinout-simplified-no-more-confusion-when-choosing-gpios/)
* [Pin map](https://cdn.techtonions.com/wp-content/uploads/2022/01/Detail-Pins-Overview-Of-ESP32.pdf?_gl=1*17o64c0*_ga*MTAyODc4OTI3OC4xNjc0Njc0NjM5*_ga_H8ZZ168JCQ*MTY3NDY3NDYzOS4xLjAuMTY3NDY3NDYzOS4wLjAuMA..)
