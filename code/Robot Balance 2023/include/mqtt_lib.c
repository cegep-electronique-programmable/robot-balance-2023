/*
!Author : Sorl_Sintoural

? Description : MQTT info for `robot-balance-2023`
* Notes : 
*/

// ----------------------------- WIFI Functions 

/* MQTT library from PlatformIO */
#include <PubSubClient.h>

/* MQTT server infos */
#define MQTT_HOST "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "cm/robot/balance"
const char *mqtt_server = MQTT_HOST;

// MQTT client creation
#define MQTT_ID "Balance"
PubSubClient clientMQTT(clientWIFI);

// ----------------------------- MQTT Functions 

/* Receive and print messages from MQTT */
void callbackMQTT(char *topic, byte *payload, unsigned int length)
{
   Serial.println();
   Serial.println(" - - - New Message - - -");
   Serial.print("Message arrived [");
   Serial.print(topic);
   Serial.print("] ");

   for (int i = 0; i < length; i++)
   {
      Serial.print((char)payload[i]);
   }

   Serial.println();
   Serial.println(" - - - END of Message - - -");
   Serial.println();
}

//! The only function to put in the main/setup() for MQTT
void setupMQTT()
{
   clientMQTT.setServer(MQTT_HOST, MQTT_PORT);
   clientMQTT.setCallback(callbackMQTT);
   clientMQTT.connect(MQTT_ID);
}

//! The only function to put in the main/loop() for MQTT
void reconnectMQTT()
{
   // Stay 
   clientMQTT.loop();

   // Check connection
   if (!clientMQTT.connected())
   // Attempt to connect
   {
      Serial.print("Attempting MQTT connection...");
      if (clientMQTT.connect(MQTT_ID))
      // Connection successful
      {
         Serial.println("connected");
         delay(3000);
      } else
      // Connection failed
      {
         Serial.print("failed, rc=");
         Serial.print(clientMQTT.state());
         Serial.println(" try again in 3 seconds");
         delay(3000);
      }
   }
}

/* Publish to MQTT */
//* clientMQTT.publish(MQTT_TOPIC, data);

