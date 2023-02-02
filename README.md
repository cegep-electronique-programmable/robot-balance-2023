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

| # Pin | GPIO | Mode | Signal | Commentaire | Strapping |
| --- | --- | --- | --- | --- | --- |
| Pin 1 | N/A | Alimentation | `GND` | | |
| Pin 2 | N/A | Alimentation | `3V3` | | |
| Pin 3 | N/A | Contrôle | `EN` | Enable | |
| Pin 4 | GPIO 36 |   |
| Pin 5 | GPIO 39 |   |
| Pin 6 | GPIO 34 |   |
| Pin 7 | GPIO 35 |   |
| Pin 8 | GPIO 32 |   |
| Pin 9 | GPIO 33 |   |
| Pin 10 | GPIO 25 |   |
| Pin 11 | GPIO 26 |   |
| Pin 12 | GPIO 27 |   |
| Pin 13 | GPIO 14 |   |  |  |  |
| Pin 14 | GPIO 12 |   |  |  | X |
| Pin 15 | N/A | Alimentation | `GND` | | |
| Pin 16 | GPIO 13 | LED | `USER_LED` |  |  |
| Pin 17 | GPIO 9 | Flash interne |  | Ne pas réutiliser |  |
| Pin 18 | GPIO 10 | Flash interne |  | Ne pas réutiliser |  |
| Pin 19 | GPIO 11 | Flash interne |  | Ne pas réutiliser |  |
| Pin 20 | GPIO 6 | Flash interne |  | Ne pas réutiliser |  |
| Pin 21 | GPIO 7 | Flash interne |  | Ne pas réutiliser |  |
| Pin 22 | GPIO 8 | Flash interne |  | Ne pas réutiliser |  |
| Pin 23 | GPIO 15 |  |  |  | X |
| Pin 24 | GPIO 2 |   |  |  | X |
| Pin 25 | GPIO 0 | Démarrage | `BOOT` | Boot, ajouter un bouton au signal `GND` | X |
| Pin 26 | GPIO 4 |   |
| Pin 27 | GPIO 16 |   |
| Pin 28 | GPIO 17 |   |
| Pin 29 | GPIO 5 | VSPI | `VSPI_CS` | Chip Select SPI | X |
| Pin 30 | GPIO 18 | VSPI | `VSPI_CLK` | Horloge SPI | |
| Pin 31 | GPIO 19 | VSPI | `VSPI_MISO` | Entrée SPI | |
| Pin 32 | N/A | NC | Aucun | Non connectée | |
| Pin 33 | GPIO 21 | I2C | `I2C_SDA` | Signal de données I2C | |
| Pin 34 | GPIO 3 | UART | `RXD0` | Broche de reception | |
| Pin 35 | GPIO 1 | UART | `TXD0` | Broche d'émission. Ajouter une résistance de 499 en série ? [HDG](https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_en.pdf) | |
| Pin 36 | GPIO 22 | I2C | `I2C_SCL` | Signal d'horloge I2C | |
| Pin 37 | GPIO 23 | VSPI | `VSPI_MOSI` | Sortie SPI | |
| Pin 38 | N/A | Alimentation | `GND` | | |
| Pin 39 | N/A | Alimentation | `GND` | | |




### SPI 🟠

SPI 0 et 1 sont utilisés à l'interne du module.

On peut utiliser SPI2 (HSPI) ou SPI3 (VSPI). J'ai choisi HSPI pour concerver le I2C sur les broches 36, 37.

[ESP32 Manuel de Référence Technique - Section 7](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#spi)

Les infos sur le shcéma adafruit placent les MOSI à la pin 30 (GPIO 18), MISO à la pin 31 (GPIO 19) et SCK à la pin 29 (GPIO 5)

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

# Références : 
* [ESP32 DevKitC v4-sch](https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf)
* [ESP32 Adafruit Huzzah32](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather)
* [HDG - Hardware Design Guideline](https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_en.pdf)
* [ESP32_WROOM datasheet](https://www.mouser.com/datasheet/2/891/esp-wroom-32_datasheet_en-1223836.pdf)
* [Blog: ESP32 Pinout simplified: No more confusion when choosing GPIOs.](https://www.techtonions.com/esp32-pinout-simplified-no-more-confusion-when-choosing-gpios/)
* [Pin map](https://cdn.techtonions.com/wp-content/uploads/2022/01/Detail-Pins-Overview-Of-ESP32.pdf?_gl=1*17o64c0*_ga*MTAyODc4OTI3OC4xNjc0Njc0NjM5*_ga_H8ZZ168JCQ*MTY3NDY3NDYzOS4xLjAuMTY3NDY3NDYzOS4wLjAuMA..)
