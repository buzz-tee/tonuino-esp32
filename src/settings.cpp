#include "settings.h"

#include <SPIFFS.h>
#include <AutoConnect.h>

#define SETTINGS_FILE "/tonuino.json"

ACText(header_settings, "Tonuino Server Settings");
ACInput(url, "https://mytonuino/", "Server URL");
ACInput(key, "", "Server Key");
ACSubmit(save, "Save", "/tonuino/server");
AutoConnectAux AuxServerSettings("/tonuino/server", "Tonuino Server", true, {header_settings, url, key, save});
ACText(settings_saved, "Settings saved!");

String onServerSettings(AutoConnectAux &aux, PageArgument &args)
{
    AuxServerSettings.release("settings_saved");
    if (!args.hasArg("_acuri"))
    {
        return String("");
    }
    SPIFFS.begin();
    Serial.println("Saving to settings file!");
    File settings = SPIFFS.open(SETTINGS_FILE, "w");
    AuxServerSettings.saveElement(settings, {"url", "key"});
    settings.close();
    SPIFFS.end();
    AuxServerSettings.add(settings_saved);

    return String("");
}

void registerSettings(AutoConnect &ac)
{
    ac.join({AuxServerSettings});
    ac.on("/tonuino/server", onServerSettings, AC_EXIT_AHEAD);

    SPIFFS.begin(true);
    if (SPIFFS.exists(SETTINGS_FILE))
    {
        Serial.println("Loading from settings file!");
        File settings = SPIFFS.open(SETTINGS_FILE, "r");
        AuxServerSettings.loadElement(settings, {"url", "key"});
        settings.close();
        SPIFFS.end();
    }
}

String getSetting(const String &key)
{
    AutoConnectElement *el = AuxServerSettings.getElement(key);
    if (el)
    {
        return el->value;
    }
    else
    {
        return String("");
    }
}

String getUrl(const String &subPath) {
    String url = getSetting("url");
    if (!url.endsWith("/")) url += "/";
    return url + subPath;
}