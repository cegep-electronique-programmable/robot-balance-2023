/*
!Author : Sorl_Sintoural

? Description : MQTT info for `robot-balance-2023`
* Notes : 
*/

/*
* Get messages from a specific topic
*  Put message in payload
*/
void callbackMQTT(char *topic, byte *payload, unsigned int length);

/* 
* All in one function for mqtt
*/
void setupMQTT();

/* 
* Reconnect to MQTT
*/
void reconnectMQTT();

/*
! TO PUT IN MAIN.CPP

	//? MQTT_TOPIC to communicate with it

setupMQTT(); //? in setup{}

reconnectMQTT(); //? in loop{}

clientMQTT.publish(MQTT_TOPIC, data); //? if want to publish

*/
