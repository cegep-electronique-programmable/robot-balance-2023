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

| Pin | Mode | Signal | Commentaire |
| --- | --- | --- | --- |
| 1 | Alimentation | `GND` | |
| 2 | Alimentation | `3V3` | |
| 3 | Contrôle | `EN` | Enable |
| 4 |   |
| 5 |   |
| 6 |   |
| 7 |   |
| 8 |   |
| 9 |   |
| 10 |   |
| 11 |   |
| 12 |   |
| 13 |   |
| 14 |   |
| 15 | Alimentation | `GND` | |
| 16 | Debug | `USER_LED` | Essayer de garde cette broche pour la DEL utilisateur |
| 17 |   |
| 18 |   |
| 19 |   |
| 20 |   |
| 21 |   |
| 22 |   |
| 23 |   |
| 24 |   |
| 25 | Déamrrage | `BOOT` | Boot |
| 26 |   |
| 27 |   |
| 28 |   |
| 29 | SPI | `SPI_SCK` | |
| 30 | SPI | `SPI_MOSI` | |
| 31 | SPI | `SPI_MISO` | |
| 32 | NC | Aucun | Non connectée |
| 33 |   |
| 34 | UART | `RXD0` | Broche de reception |
| 35 | UART | `TXD0` | Broche d'émission |
| 36 | I2C | `I2C_SCL` | Signal d'horloge I2C |
| 37 | I2C | `I2C_SDA` | Signal de données I2C |
| 38 | Alimentation | `GND` | |
| 39 | Alimentation | `GND` | |

RéférenceS : 
* [ESP32 DevKitC v4-sch](https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf)
* [ESP32 Adafruit Huzzah32](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather)
* [Hardware Design Guideline](https://www.espressif.com/sites/default/files/documentation/esp32_hardware_design_guidelines_en.pdf)

## Firmware

* [ ] Section à compléter
