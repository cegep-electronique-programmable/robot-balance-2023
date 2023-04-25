#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

AsyncWebServer server(80); // Create AsyncWebServer object on port 80

// Fonction qui permet de recevoir les données envoyées par le client par le webserial
void recvMsg(uint8_t *data, size_t len)
{
    WebSerial.println("Received Data...");
    String d = "";
    for (int i = 0; i < len; i++)
    {
        d += char(data[i]);
    }
    WebSerial.println(d);
}

#define WIFI_ACTIVE ENTERPRISE // Commenter cette ligne pour désactiver le wifi

#include "secrets.h"

#ifdef WIFI_ACTIVE
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
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

void initWifi()
{

#ifdef WIFI_ACTIVE
    WiFi.mode(WIFI_STA);
#if WIFI_ACTIVE != ENTERPRISE
    // Connection WiFi si nous sommes dans un réseau wifi normal (non entreprise)
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
#else
    // Connection WiFi si nous sommes dans un réseau wifi entreprise
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_wpa2_ent_enable();
    WiFi.begin(ssid);
    // Attente de la connection au réseau wifi
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        counter++;
        breathingEffect(pixels.Color(255, 0, 0));
        // Si la connection prend plus de 30 secondes, on redémarre l'ESP32
        if (counter >= 60)
        {
            ESP.restart(); // Redémarrage de l'ESP32, donc pas besoin d'appuyer sur le bouton reset
        }
    }
#endif
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    pixels.setBrightness(0x22);
    for (int i = 0; i < NEOPIXEL_COUNT; i++)
    {
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    }
    pixels.show();
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
      
      if (ArduinoOTA.getCommand() == U_FLASH) // U_FLASH est pour modifier le code de l'ESP32
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
    // Start le serveur web
    WebSerial.begin(&server);
    WebSerial.msgCallback(recvMsg);
    server.begin();
    delay(250);
    for (int i = 0; i < NEOPIXEL_COUNT; i++)
    {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
    
}