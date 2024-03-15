#include <Arduino.h>

#include "init.h"
#include "config.h"
#include "board_mapping.h"

// ***************  LED  *************** //


#if NEOPIXEL_ACTIVE == 1
Adafruit_NeoPixel pixels(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
int pixel = 0;
#endif


// *******  Boucle compensation  ******* //  
const float Kp = 0.5;
const float Ki = 0.5;
const float Kd = 0.5;


// ********  Contreole vitesse  ******** //  
#define INCREMENT_VITESSE 10

// ************************************* //

void setup() {
  int initilisation_reussie = 0;
  initilisation_reussie += initialisationsNeoPixel(pixels);
  initilisation_reussie += initialisationSerie();
  initilisation_reussie += initialisationBroches();
  initilisation_reussie += initialisationPWMMoteurs();
  initilisation_reussie += initialisationI2C();


}


void getInfoMouvement(structInfoMouvement * myStructInfoMouvement){

  byte dataCMPS12[19];
  int iFori2c = 0;
  int IncliParCMPSReg4 = 0;
  int IncliParCalcul = 0;
  int incli = 0;
  int accelY = 0;
  int accelZ = 0;
  int gyroX = 0;



  //Lire le compas
  Wire.beginTransmission(CMPS12_I2C_ADDR);    // Get the slave's attention, tell it we're sending a command byte
  Wire.write(0x00);                           // Envoyer 0x04 au magnétomètre pour dire qu'on veut lire le registre 4, donc le pitch
  Wire.endTransmission(false);                // "Hang up the line" so others can use it (can have multiple slaves & masters connected)
  Wire.requestFrom(CMPS12_I2C_ADDR, 19, (int)true);    // Tell slave we need to read 1byte from the current register
  iFori2c = 0;
  while ( Wire.available() != 0 ){
    dataCMPS12[iFori2c] = Wire.read();                   // read that byte into 'slaveByte' variable  

    iFori2c++;
    if (iFori2c == 19){
      break;
    }
  }  

  IncliParCMPSReg4 = dataCMPS12[4];
  //printf("Inclinaison par le registre 4: %d (%x)\r\n", IncliParCMPSReg4, IncliParCMPSReg4);
  if (dataCMPS12[4] & 0x80){
    IncliParCMPSReg4 = 0xFF00 | IncliParCMPSReg4;
  }
  //printf("Inclinaison par le registre 4: %d (%x)\r\n", IncliParCMPSReg4, IncliParCMPSReg4);

  accelY = (dataCMPS12[14] << 8) | dataCMPS12[15];
  //printf("AccelY: %d (%x)\r\n", accelY, accelY);

  accelZ = (dataCMPS12[16] << 8) | dataCMPS12[17];
  //printf("AccelZ: %d (%x)\r\n", accelZ, accelZ);

  gyroX = (dataCMPS12[18] << 8) | dataCMPS12[19];
  printf("GyroX: %d (%x)\r\n", gyroX, gyroX);
  float angulare_speed_from_gyro = (float)gyroX * 2000.0 / (float)0x7FFF;
  printf("angulare_speed_from_gyro: %d (%x)\r\n\n", angulare_speed_from_gyro, angulare_speed_from_gyro);


  myStructInfoMouvement->inclinaison = IncliParCMPSReg4;
  myStructInfoMouvement->vitesse = gyroX;


/*
    // Lire l'angle x du magnétomètre MXC6655XA
    Wire.beginTransmission(MXC6655XA_I2C_ADDR);
    Wire.write(MXC6655XA_I2C_XOUT_REG); // Register 3: X-axis MSB
    Wire.endTransmission();

    // Request 1 bytes from MXC6655XA
    int nReceived = 0;
    nReceived = Wire.requestFrom(MXC6655XA_I2C_ADDR, MXC6655XA_I2C_XOUT_REG_LENGTH);

    nReceived = Wire.readBytes(mxc6655xaXout, MXC6655XA_I2C_XOUT_REG_LENGTH);

    // Something has gone wrong
    if (nReceived != MXC6655XA_I2C_XOUT_REG_LENGTH)
    {
      printf("Erreur de reception MXC6655XA\r\n");
    }
    else{
      printf("Données reçues: ");
      for (int i = 0; i < MXC6655XA_I2C_XOUT_REG_LENGTH; i++)
      {
        printf("0x%02X ", mxc6655xaXout[i]);
      }
      printf("\r\n");
    
    }
    */  

}


float bouclePID(structInfoMouvement * myStructInfoMouvement)
{
  int erreurActuelle = 0;
  float integrale = 0;
  float derivee = 0;
  static int erreurPrecedente = 0;
  float tempsActuel = micros();
  static float tempsPrecedent = 0;
  float vitesseCible = 0;

  erreurActuelle = 0 - myStructInfoMouvement->inclinaison;

  integrale = integrale + ((float) erreurActuelle * (tempsActuel - tempsPrecedent));

  derivee = (float)(erreurActuelle - erreurPrecedente) / (tempsActuel - tempsPrecedent);

  vitesseCible = Kp * erreurActuelle + Ki * integrale + Kd * derivee;
}


void commandeMoteurs(float vitessCible)
{
  static int vitesseActuelle = 0;


  if (vitessCible > vitesseActuelle){
    //On doit augmenter la vitesse du moteur
    if ((vitesseActuelle > (0 - INCREMENT_VITESSE)) && (vitesseActuelle < 0)){

    }
    else if (vitesseActuelle < vitessCible){
      //On doit diminuer la vitesse du moteur
      vitesseActuelle++;
    }
  }

}


void loop() {
  int test = 0;
  uint8_t mxc6655xaXout[MXC6655XA_I2C_XOUT_REG_LENGTH];

  struct structInfoMouvement myStructInfoMouvement = {0, 0};
  float vitessCible = 0;

  while(1){

    printf("%i\r\n", test++);

    //pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    //pixels.show();    
    
    getInfoMouvement(&myStructInfoMouvement);

    vitessCible = bouclePID(&myStructInfoMouvement);

    commandeMoteurs(vitessCible);


    //printf("incli: %d, vitesse: %d\r\n\n", myStructInfoMouvement.inclinaison, myStructInfoMouvement.vitesse);

    //ledcChangeFrequency(PWM_CHANNEL_0, PWM_FREQ_INIT, PWM_RESOLUTION);

    delay(1000);              // 1 sec
  
  }

  //Test commit from GitHub

}


/*
    Wire.beginTransmission(CMPS12_I2C_ADDR);    // Get the slave's attention, tell it we're sending a command byte
    Wire.write(0x01);                           // Envoyer 0x01 au magnétomètre pour dire qu'on veut lire le registre 1, donc le compas
    Wire.endTransmission(false);                // "Hang up the line" so others can use it (can have multiple slaves & masters connected)
    Wire.requestFrom(CMPS12_I2C_ADDR, 2, (int)true);    // Tell slave we need to read 1byte from the current register
    while ( Wire.available() != 0 ){
      printf("%d octets disponibles\r\n", Wire.available());
      angleActuelle = Wire.read();                   // read that byte into 'slaveByte' variable  
      Serial.print("Byte reçu 0x");
      Serial.println(angleActuelle, HEX);
      Serial.print("\n");
    }
*/
