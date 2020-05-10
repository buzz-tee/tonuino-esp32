#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

#include "settings.h"
#include "player.h"
#include "cardreader.h"
#include "controller.h"

CardReader cardReader;
Player audioPlayer;
Controller controller;

WebServer server;
AutoConnect portal(server);
AutoConnectConfig config("", "12345678");

void onVolumeUp()
{
    Serial.println("Volume up!");
    audioPlayer.volumeUp();
}

void onVolumeDown()
{
    Serial.println("Volume down!");
    audioPlayer.volumeDown();
}

void onPause()
{
    Serial.println("Pause!");
    audioPlayer.pause();
}

void onNext()
{
    Serial.println("Next!");
    audioPlayer.next();
}

void onPrevious()
{
    Serial.println("Previous!");
    audioPlayer.previous();
}

void setup()
{
    Serial.begin(115200);
    Serial.println();

    String softApName = "Tonuino_" + WiFi.macAddress().substring(9);
    softApName.replace(":", "");
    config.apid = softApName.c_str();
    config.apip = IPAddress(172, 31, 28, 1);
    config.gateway = IPAddress(172, 31, 28, 1);
    config.title = "Tonuino Setup";
    config.hostName = "tonuino";
    config.ota = AC_OTA_BUILTIN;
    portal.config(config);

    registerSettings(portal);

    portal.disableMenu(AC_MENUITEM_HOME);

    if (portal.begin())
    {
        Serial.println("WiFi connected: " + WiFi.localIP().toString());
    }

    cardReader.begin();
    audioPlayer.begin();
    controller.begin();
    controller.setVolumeUpCallback(onVolumeUp);
    controller.setVolumeDownCallback(onVolumeDown);
    controller.setPauseCallback(onPause);
    controller.setNextCallback(onNext);
    controller.setPreviousCallback(onPrevious);
}

void readCard()
{
    cardReader.handleCard();
    if (cardReader.isCardPresent())
    {
        if (cardReader.isNewCardPresent())
        {
            // New card -> start playing
            char buffer[256];
            if (cardReader.readCard(buffer, 256) > 0) {
                String url = getSetting("url");
                if (!url.endsWith("/")) url += "/";
                url += buffer;
                audioPlayer.playlist(url.c_str());
            }
        }
        else
        {
            // Still the same card -> nothing to do
        }
    }
    else
    {
        if (audioPlayer.isPlaying())
        {
            // Card removed -> stop playing
            audioPlayer.stop(true);
        }
    }
}

void loop()
{
    portal.handleClient();

    readCard();

    controller.loop();

    delay(100);
}
