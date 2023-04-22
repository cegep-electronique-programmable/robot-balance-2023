/*
!Author : Sorl_Sintoural

? Description : MQTT info for `robot-balance-2023`
* Notes : 
*/

/* MQTT library from PlatformIO */
#include <PubSubClient.h>

/* MQTT server infos */
#define MQTT_HOST "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_TOPIC "cm/robot/balance"
const char *mqtt_server = MQTT_HOST;

// MQTT client creation
#define MQTT_ID "Robot"
#define MQTT_USER "Balance"
PubSubClient client(espClient);

// ----------------------------- MQTT Functions for SETUP()

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

void setupMQTT()
{
   client.setServer(mqtt_server, 1883);
   client.setCallback(callbackMQTT);
}

//! PUT AT END OF SETUP !//
void initPrint()
{
   Serial.println("- - - - - - - - - - - - - Initialized - - - - - - - - - - - - - ");
}

// ----------------------------- END of MQTT SETUP() section


// ----------------------------- MQTT Functions in LOOP()

/* Reconnect to MQTT */
void reconnectMQTT()
{
   // Check connection
   if (!client.connected())
   // Attempt to connect
   {
      Serial.print("Attempting MQTT connection...");
      if (client.connect(MQTT_ID))
      // Connection successful
      {
         Serial.println("connected");
         delay(3000);
      } else
      // Connection failed
      {
         Serial.print("failed, rc=");
         Serial.print(client.state());
         Serial.println(" try again in 3 seconds");
         delay(3000);
      }
   }
}

/* Publish to MQTT */
//* client.publish(MQTT_TOPIC, data);

// ----------------------------- END of MQTT LOOP() section
