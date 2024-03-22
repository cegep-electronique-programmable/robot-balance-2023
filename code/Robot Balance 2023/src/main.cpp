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


  int8_t dataCMPS12[19];
  int16_t iFori2c = 0;
  int16_t IncliParCMPSReg4 = 0;
  int16_t IncliParCalcul = 0;
  int16_t incli = 0;
  int16_t accelY = 0;
  int16_t accelZ = 0;
  int16_t gyroX = 0;
  float angulare_speed_from_gyro = 0;



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

  if (19 != iFori2c){
    printf("CMPS12 ERR\r\n");
  }
  else{
    IncliParCMPSReg4 = (int) dataCMPS12[4];
    accelY = (dataCMPS12[14] << 8) | dataCMPS12[15];
    accelZ = (dataCMPS12[16] << 8) | dataCMPS12[17];
    gyroX = (dataCMPS12[18] << 8) | dataCMPS12[19];
    float angulare_speed_from_gyro = (float)gyroX * 2000.0 / (float)0x7FFF;

    myStructInfoMouvement->inclinaison = IncliParCMPSReg4;
    myStructInfoMouvement->vitesse = gyroX;

    //printf pour inclinaison
    //printf("IncliParCMPSReg4;%d;", IncliParCMPSReg4); //Conversion validé !  
    //printf("accelY;%d;", accelY);
    //printf("accelZ;%d;", accelZ);
    //printf("IncliParAccel;%d;", IncliParAccel);

    //printf pour vitesse
    printf("angulare_speed_from_gyro;%f;", angulare_speed_from_gyro);  //Conversion validé !  

    printf("\r\n");    
  }








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

//Pour tester la vitesse des moteurs, appeler cette fonction chaque milliseconde
void testVitesseMoteurs(void){
  static int freq_actuel = 10;
  static int etat = 0;  // 0 pour accel - 1 pour attente - 2 pour decel - 3 pour attente 
  static int attente = 0;

  switch (etat)
  {
    case 0:
      if (freq_actuel < 6000){
        freq_actuel +=40;
      }
      else{
        etat = 1;
        printf("etat 1 - freq == %d\r\n", freq_actuel);
      }
      break;
    case 1:
      if (attente < 500){
        attente +=1;
      }
      else{
        attente = 0;
        etat = 2;
        printf("etat 2\r\n");
      }    
      break;
    case 2:
      if (freq_actuel > 10){
        freq_actuel -= 10;
      }
      else{
        etat = 3;
        printf("etat 3 - freq == %d\r\n", freq_actuel);
      }
      break;  
    case 3:
      if (attente < 2000){
        attente +=1;
      }
      else{
        attente = 0;
        etat = 0;
        printf("etat 0\r\n");
      }    
      break;            
    default:
      break;
  }



  ledcChangeFrequency(PWM_CHANNEL_D, freq_actuel, PWM_RESOLUTION);
  ledcChangeFrequency(PWM_CHANNEL_G, freq_actuel, PWM_RESOLUTION);
}


void loop() {
  int test = 0;
  uint8_t mxc6655xaXout[MXC6655XA_I2C_XOUT_REG_LENGTH];

  struct structInfoMouvement myStructInfoMouvement = {0, 0};
  float vitessCible = 0;

  while(1){

    //printf("%i\r\n", test++);

    //pixels.setPixelColor(3, pixels.Color(255, 0, 0));
    //pixels.show();    
    
    getInfoMouvement(&myStructInfoMouvement);

    //vitessCible = bouclePID(&myStructInfoMouvement);

    //commandeMoteurs(vitessCible);
    //testVitesseMoteurs();

    delay(200);              // délais en ms
  
  }


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
