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

| Pin | Description |
| --- | --- |
| 1 | GND |
| 2 | 3V3 |
| 3 | EN |
| 4 |  |
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
| 15 | GND |
| 16 |   |
| 17 |   |
| 18 |   |
| 19 |   |
| 20 |   |
| 21 |   |
| 22 |   |
| 23 |   |
| 24 |   |
| 25 |   |
| 26 |   |
| 27 |   |
| 28 |   |
| 29 |   |
| 30 |   |
| 31 |   |
| 32 | Non connectée |
| 33 |   |
| 34 |   |
| 35 | TX |
| 36 | RX |
| 37 |   |
| 38 | GND |
| 39 | GND |

RéférenceS : 
* [esp32_devkitc_v4-sch](https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf)

## Firmware

* [ ] Section à compléter
