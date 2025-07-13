// MAX7219 RDA Message Board Version
// Tested on "ESP8266 Boards (3.0.2) / NodeMCU 1.0 (ESP12E-Module)"
// Also tested on "Wemos/LoLin D1 Mini"

#include <MD_MAX72xx.h>    // by MajicDesign
#include <MD_Parola.h>     // by MajicDesign
#include <SPI.h>           // Included in ESP8266
#include <ESP8266mDNS.h>   // Included in ESP8266
#include <WiFiManager.h>   // by tzapu
#include <EasyButton.h>    // by Evert Arias, ver 2.0.3
#include <stdio.h>
#include <ArduinoJson.h>   // by Benoit Blanchon, ver 6.21.5
#include <LittleFS.h>      // Included in ESP8266
#include <PubSubClient.h>  // by Nick O'Leary 
#include "01_Shared.h"

#define VERSION "v2025.04.11"

void setup() {
  clientId = clientIdPrefix + chipId;// + clientIdTrail;
  clientId.toUpperCase();
  clientId.toCharArray(mqttTopicDevice, 128);

  // WiFi Config Portal - AP Mode WiFi Details
  ap_mode_ssid = clientId.c_str();
  ap_mode_password = "wifi-setup";

#if DEBUG
  Serial.begin(115200);
  PRINTS("\n[MD_MAX72XX WiFi Message Display]\nType a message for the scrolling display from your internet browser");
#endif
  Serial.println("");

#if ENABLE_FLASH_BUTTON
  flash_button.begin();
  // Add the callback function to be called when the button is pressed.
  flash_button.onPressed(onPressed);
#endif

  // Initialize LittleFS library
  while (!LittleFS.begin()) {
    Serial.println(F("Failed to initialize LittleFS library"));
    delay(1000);
  }

  // You can use the remove lines below to delete existing config files, (especially if config is invalid and crasing device)
  //LittleFS.remove(webConfigFile);
  //LittleFS.remove(mqttConfigFile);
  
  // Initialize config files
  initWebStoreConfig();
  initMqttStoreConfig();

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  // Display initialisation
  P.begin();

  P.addChar('¬', degC);
  //P.addChar('&', degF);
  //P.addChar('~', waveSine);
  //P.addChar('+', waveSqar);
  //P.addChar('^', waveTrng);

  curMessage[0] = newMessage[0] = '\0';

  WiFi.mode(WIFI_STA);  // Explicitly set mode, esp defaults to STA+AP
  
  WiFiManager wm;       // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around

  // Comment out and edit line below if you want to change AP Config Portal IP from default 192.168.4.1 to something else
  //wm.setAPStaticIPConfig(IPAddress(192,168,100,1), IPAddress(192,168,100,1), IPAddress(255,255,255,0));

  //wm.setSTAStaticIPConfig(IPAddress(192,168,1,92), IPAddress(192,168,1,1), IPAddress(255,255,255,0), IPAddress(192,168,1,1));  // Optional DNS 4th argument

  wm.setHostname(clientId);

  // Run code before WiFi setup is checked/started
  wm.setAPCallback(configModeCallback);
  wm.setClass("invert");  // Dark theme

  // Confiugre WifiManager portal (essentially specifying the button we want on the captive portal page)
  std::vector<const char *> wm_menu  = {"wifi", "info", "update"};
  wm.setShowInfoUpdate(true);
  wm.setShowInfoErase(true);
  wm.setMenu(wm_menu);

  // Try to connect for 60 seconds before starting config portal
  wm.setConnectTimeout(60);

  bool res;
  // res = wm.autoConnect();                               // Auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP");                // Anonymous ap
  res = wm.autoConnect(ap_mode_ssid,ap_mode_password);  // Start AP either in setup mode or connects to configured wifi network
  if(!res) {
    Serial.println("Failed to connect");
  }
  else {
    // If you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    firstMessage = true;
    if(afterWiFiConfig){
      ESP.restart();
    }
  }

  // Show ip address on serial
  sprintf(assignedIP, "%01d.%01d.%01d.%01d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  PRINT("\nAssigned IP: ", assignedIP);
  PRINTS("\n\n");

  httpWebDirDef();

  // Scroll first message in message mode
  sprintf(newMessage, "Wifi Message Mode - Hostname: %s - IP: %s - http Username: %s - Password: %s - Version: %s", clientId.c_str(), assignedIP, web_username, web_password, VERSION);
  startupBuzzer();
  displaySilentMsg();

  if (MDNS.begin(clientId)) {
    MDNS.addService("http", "tcp", 80);
    PRINT("\n\nmDNS responder started correctly, name: ", clientId);
  }
  else {
    PRINTS("\n\nError setting up MDNS responder!");
  }
  initMqtt();
}


void loop() {
  MDNS.update();
  handleHttpServer();
  checkMqtt();
  scrollTextParola(); 

#if ENABLE_FLASH_BUTTON
  flash_button.read();
#endif
}
