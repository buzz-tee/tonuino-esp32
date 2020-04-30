#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

#include "settings.h"
#include "player.h"
#include "cardreader.h"

CardReader  cardReader;
Player      audioPlayer;

WebServer   server;
AutoConnect portal(server);
AutoConnectConfig config("", "12345678");

void setup() {
  Serial.begin(115200);
  Serial.println();

  String softApName = "Tonuino_" + WiFi.macAddress().substring(9);
  softApName.replace(":", "");
  config.apid = softApName.c_str();
  config.apip = IPAddress(172,31,28,1);
  config.gateway = IPAddress(172,31,28,1);
  config.title = "Tonuino Setup";
  portal.config(config);

  registerSettings(portal);

  portal.disableMenu(AC_MENUITEM_HOME);

  if (portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

  cardReader.begin();
  audioPlayer.begin();
}

void readCard() {
  cardReader.handleCard();
  if (cardReader.isCardPresent()) {
    if (cardReader.isNewCardPresent()) {
      // New card -> start playing
      audioPlayer.start("http://192.168.10.2/tonuino/Highwayman.mp3");
    } else {
      // Still the same card -> nothing to do
    }
  } else {
    if (audioPlayer.isPlaying()) {
      // Card removed -> stop playing
      audioPlayer.stop();
    }
  }
}

void loop() {
  portal.handleClient();

  readCard();

  delay(1000);
}
