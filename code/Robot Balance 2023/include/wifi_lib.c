/*
!Author : Sorl_Sintoural

? Description : WIFI info for `robot-balance-2023`
* Notes :
*/

#define WIFI_ACTIVE ENTERPRISE

#include "secrets.h"

#ifdef WIFI_ACTIVE
	#include <WiFi.h>
	#include <WiFiUdp.h>
	#if WIFI_ACTIVE != ENTERPRISE
		const char *host = WIFI_HOST_NAME;
		const char *ssid = WIFI_SSID;
		const char *password = WIFI_PASSWORD;
	#else
		#include "esp_wpa2.h"
		const char *ssid = EAP_SSID;
		int counter = 0;
	#endif
#endif

//////////////////////////////////////////////////////////////////////////

// ----------------------------- WIFI Functions for SETUP()



// ----------------------------- Connecting to WIFI
String translateEncryptionType(wifi_auth_mode_t encryptionType)
{
	switch (encryptionType)
	{
	case (WIFI_AUTH_OPEN):
		return "Open";
	case (WIFI_AUTH_WEP):
		return "WEP";
	case (WIFI_AUTH_WPA_PSK):
		return "WPA_PSK";
	case (WIFI_AUTH_WPA2_PSK):
		return "WPA2_PSK";
	case (WIFI_AUTH_WPA_WPA2_PSK):
		return "WPA_WPA2_PSK";
	case (WIFI_AUTH_WPA2_ENTERPRISE):
		return "WPA2_ENTERPRISE";
	default:
		return "UNKNOWN";
	}
	return "UNKNOWN";
}

void scanNetworks()
{
	int numberOfNetworks = WiFi.scanNetworks();

	Serial.print("Number of networks found: ");
	Serial.println(numberOfNetworks);

	for (int i = 0; i < numberOfNetworks; i++)
	{

		Serial.print("Network name: ");
		Serial.println(WiFi.SSID(i));

		Serial.print("Signal strength: ");
		Serial.println(WiFi.RSSI(i));

		Serial.print("MAC address: ");
		Serial.println(WiFi.BSSIDstr(i));

		Serial.print("Encryption type: ");
		String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
		Serial.println(encryptionTypeDescription);
		Serial.println("-----------------------");
	}
}

//! The only function to put in the main/setup() for WIFI
void connectToNetwork()
{
	scanNetworks();

	#ifdef WIFI_ACTIVE
		WiFi.mode(WIFI_STA);
		#if WIFI_ACTIVE != ENTERPRISE
		WiFi.begin(ssid, password);

		Serial.println("Connecting to wifi");
		while (WiFi.status() != WL_CONNECTED)
		{
			Serial.printf("Status: %d\r\n", WiFi.status());
			WiFi.printDiag(Serial);
			Serial.printf("TxPower: %d\r\n", WiFi.getTxPower());

			Serial.print(".");
			delay(100);
		}

		Serial.printf("Local IP address: %s\r\n", String(WiFi.localIP()));

		Serial.println("Connected to network");
		#else
			WiFi.disconnect(true);
			WiFi.mode(WIFI_STA);
			esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
			esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
			esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
			esp_wifi_sta_wpa2_ent_enable();
			WiFi.begin(ssid);

		#endif

		reconnectWIFI();

		Serial.println("Ready");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());

		WiFiClient clientWIFI;
	#endif
}

//! The only function to put in the main/loop() for WIFI
void reconnectWIFI()
{
	#if WIFI_ACTIVE != ENTERPRISE
		while (WiFi.waitForConnectResult() != WL_CONNECTED)
		{
			Serial.println("Connection Failed! Rebooting...");
			delay(5000);
			ESP.restart();
		}
	#else 
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(500);
			Serial.print(".");
			counter++;
			if (counter >= 60)
			{ // after 30 seconds timeout - reset board
				ESP.restart();
			}
		}
	#endif
}
