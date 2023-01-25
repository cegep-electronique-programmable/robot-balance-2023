# Robot Balance 2023

Informations techniques du robot qui tient en équilibre sur deux roues.
[Ajouter un photo ici]

# Présentation

Le **Robot Balance 2023** est une évolution du robot 2017. Les objectifs de la nouvelles version sont :
* Enlever les fils d'alimentation : Le robot est maintenant alimenté par une **batterie 12V**.
* Surélever le centre de gravité pour **améliorer le contrôle**. La batterie est situé en haut du robot.
* Pouvoir commander et observer le robot sans fils : La carte LPC1768 est remplacée par une carte à base de **ESP32 (Wifi)**.

# Aperçu du développement

## Mécanique 

La mécanique est développée dans Fusion 360 et elle est basée sur un cadre en extrusions d'aluminium.
Les roues sont entrainées par des moteurs pas-à-pas de type NEMA-17.

## Électronique

La carte électronique est développée dans Fusion 360 et elle est basée sur :
* Un microcontrôleur ESP32-WROOM-32.
* Deux commandes moteur A4988.
* Un capteur ADXL.
* Un capteur CMPS12.
* Des régulateurs d'alimentation xxx et yyy.

## Firmware

Le firmware est développé à l'aide de l'outil PlatformIO. Nous utilisons le framework (TBD: Arduino ou IDK).

Le contrôle se fait à l'aide d'un contrôleur de type Proportionnel-Intégral-Dérivé (PID).

# Description détaillée

## Mécanique 

* [ ] Section à compléter

## Électronique

### Composants principaux

* [ ] Section à compléter

### Choix des broches du microcontrôleur

| Pin | Mode | Signal | Commentaire | Strapping |
| --- | --- | --- | --- | --- |
| 1 | Alimentation | `GND` | | |
| 2 | Alimentation | `3V3` | | |
| 3 | Contrôle | `EN` | Enable | |
| 4 |   |
| 5 |   |
| 6 |   |
| 7 |   |
| 8 |   |
| 9 |   |
| 10 |   |
| 11 |   |
| 12 |   |
| 13 | HSPI | `HSPICLK` | Horloge HSPI | |
| 14 | HSPI | `HSPIQ` | Émission HSPI | X |
| 15 | Alimentation | `GND` | | |
| 16 | HSPI | `HSPID` | Réception HSPI | |
| 17 | Flash interne |  | Ne pas réutiliser |  |
| 18 | Flash interne |  | Ne pas réutiliser |  |
| 19 | Flash interne |  | Ne pas réutiliser |  |
| 20 | Flash interne |  | Ne pas réutiliser |  |
| 21 | Flash interne |  | Ne pas réutiliser |  |
| 22 | Flash interne |  | Ne pas réutiliser |  |
| 23 | HSPI | `HSPICS0` | Chip Select HSPI | X |
| 24 |   |  |  | X |
| 25 | Démarrage | `BOOT` | Boot, ajouter un bouton au signal `GND` | X |
| 26 |   |
| 27 |   |
| 28 |   |
| 29 |   |   |   | X |
| 30 |   |   |   |   |
| 31 |   |   |   |   |
| 32 | NC | Aucun | Non connectée | |
| 33 |   |
| 34 | UART | `RXD0` | Broche de reception | |
| 35 | UART | `TXD0` | Broche d'émission. Ajouter une résistance de 499 en série ? HRG | |
| 36 | I2C | `I2C_SCL` | Signal d'horloge I2C | |
| 37 | I2C | `I2C_SDA` | Signal de données I2C | |
| 38 | Alimentation | `GND` | | |
| 39 | Alimentation | `GND` | | |

RéférenceS : 
* [ESP32 DevKitC v4-sch](https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf)
* [ESP32 Adafruit Huzzah32](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather)
* [HRG - Hardware Design Guideline](https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_en.pdf)
* [ESP32_WROOM datasheet](https://www.mouser.com/datasheet/2/891/esp-wroom-32_datasheet_en-1223836.pdf)
* [blog](https://www.techtonions.com/esp32-pinout-simplified-no-more-confusion-when-choosing-gpios/)


### SPI 🟠

SPI 0 et 1 sont utilisés à l'interne du module.

On peut utiliser SPI2 (HSPI) ou SPI3 (VSPI). J'ai choisi HSPI pour concerver le I2C sur les broches 36, 37.

[ESP32 Manuel de Référence Technique - Section 7](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#spi)

### I2C 🔴

Différentes info :
* [Schéma Adafruit](https://learn.adafruit.com/assets/41630) :
  * SCL Pin 36 IO22
  * SDA Pin 37 IO23
* [ESP32 Manuel de Référence Technique - Section 4.11](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#spi) :
  * SCL GPIO 4 ou GPIO 2 -> Pin 26 ou 24
  * SDA GPIO 0 ou GPIO 15 -> Pin 25 ou 23
* Divers tutoriaux recommandent ([Par exemple](https://deepbluembedded.com/esp32-i2c-tutorial-change-pins-i2c-scanner-arduino/#I2C_Bus_Lines)):
  * SCL GPIO 22
  * SDA GPIO 21 
  
  ![image](https://user-images.githubusercontent.com/32198019/214665266-d51fead5-9cdc-4ce7-bf78-924b9762b46b.png)


Je ne comprends pas encore à quel point le MUX (RTC_MUX ?) permet de faire des choix par firmware.

[ESP32 Manuel de Référence Technique - Section 11](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#i2c)

### Circuit de décharge 

![image](https://user-images.githubusercontent.com/5272111/214639099-4ffefefb-0941-4416-903f-3794575252fe.png)


### Guide pour le PCB Layout

PCB à 4 couches :
| Couche | Signaux |
| -- | -- |
| 1 | Traces et composants |
| 2 | Plan de ground `GND` |
| 3 | Alimentations |
| 4 | Traces de signaux et d'alimentations (pas de composant si possible) |

Garder 15 mm d'espaces sur les 3 côtés de l'antenne.


## Firmware

* [ ] Section à compléter
