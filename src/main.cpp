#include <Arduino.h>
#include <time.h>

#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>

#include "settings.h"
#include "player.h"
#include "cardreader.h"
#include "controller.h"


#define IDLE_SLEEP_TIMEOUT      120000


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

void onReset() {
    Serial.println("RESET");
    audioPlayer.beep();
    delay(250);

    esp_restart();
}

void onParty() {
    Serial.println("Party");
    audioPlayer.beep();
    delay(250);
    // TODO
}

void printLocalTime() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
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

    configTime(0, 0, "pool.ntp.org");

    cardReader.begin();
    audioPlayer.begin();
    controller.begin();
    controller.setVolumeUpCallback(onVolumeUp);
    controller.setVolumeDownCallback(onVolumeDown);
    controller.setPauseCallback(onPause);
    controller.setNextCallback(onNext);
    controller.setPreviousCallback(onPrevious);

    controller.setResetCallback(onReset);
    controller.setPartyCallback(onParty);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        Serial.println("Wakeup from deep sleep");
        audioPlayer.beep();
    }

    Serial.print("Startup complete: "); printLocalTime();
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
            Serial.print("Stop!");
            audioPlayer.stop(true);
        }
    }
}

void loop()
{
    portal.handleClient();

    readCard();

    controller.loop();

#ifdef ENABLE_DEEP_SLEEP
    if (audioPlayer.idleSince() > IDLE_SLEEP_TIMEOUT && controller.isOnBattery()) {
        printLocalTime();
        Serial.println("Idle for long AND on battery -> going to deep sleep");

        esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);
        esp_deep_sleep_start();
    }
#endif

    delay(100);
}
