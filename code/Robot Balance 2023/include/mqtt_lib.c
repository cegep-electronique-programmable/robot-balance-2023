/*
!Author : Sorl_Sintoural

? Description : MQTT info for `robot-balance-2023`
* Notes : 
*/

#include <PubSubClient.h>

#define MQTT_HOST "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_ID "Robot"
#define MQTT_USER "Balance"
#define MQTT_TOPIC "cm/robot/balance"

// MQTT client creation
PubSubClient client(espClient);
const char *mqtt_server = MQTT_HOST;

// MQTT message creation
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
const char *msg[MSG_BUFFER_SIZE];

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

/* Reconnect to MQTT */
void reconnectMQTT()
{
   // Loop until we're reconnected
   if (!client.connected())
   {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(MQTT_ID)) // if (client.connect(MQTT_ID, MQTT_USER, MQTT_PASS))
      {
         Serial.println("connected");
         delay(3000);
      }
      else
      {
         Serial.print("failed, rc=");
         Serial.print(client.state());
         Serial.println(" try again in 3 seconds");
         // Wait 3 seconds before retrying
         delay(3000);
      }
   }
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
   // Loop until we're reconnected
   if (!client.connected())
   {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(MQTT_ID)) // if (client.connect(MQTT_ID, MQTT_USER, MQTT_PASS))
      {
         Serial.println("connected");
         delay(3000);
      }
      else
      {
         Serial.print("failed, rc=");
         Serial.print(client.state());
         Serial.println(" try again in 3 seconds");
         // Wait 3 seconds before retrying
         delay(3000);
      }
   }
}

/* Publish to MQTT */
//* client.publish(MQTT_TOPIC, data);


