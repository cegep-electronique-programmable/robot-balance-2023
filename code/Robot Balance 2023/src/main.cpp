#include <Arduino.h>

#define WIFI_ACTIVE ENTERPRISE

#ifdef WIFI_ACTIVE
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#if WIFI_ACTIVE != ENTERPRISE
const char *host = "esp32";
const char *ssid = "TBD";
const char *password = "TBD";
#else
#include "esp_wpa2.h"           //wpa2 library for connections to Enterprise networks
#define EAP_IDENTITY "tbd@cmaisonneuve.qc.ca"    // if connecting from another corporation, use identity@organisation.domain in Eduroam
#define EAP_PASSWORD "tbd" // your Eduroam password
const char *ssid = "eduroam";         // Eduroam SSID
int counter = 0;
#endif
#endif

int timeout = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");

#ifdef WIFI_ACTIVE
  WiFi.mode(WIFI_STA);
#if WIFI_ACTIVE != ENTERPRISE
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
#else
  WiFi.disconnect(true);                                                             // disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA);                                                               // init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD)); // provide password
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(ssid); // connect to wifi
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

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();

#endif
}

void loop()
{
#ifdef WIFI_ACTIVE
  ArduinoOTA.handle();
#endif

  timeout++;

  if (timeout % 1000 == 0)
  {
    Serial.println("version 5");
  }
}