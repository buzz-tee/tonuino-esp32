#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <Arduino.h>
#include <AutoConnect.h>

void registerSettings(AutoConnect& ac);
String getSetting(const String& key);

#endif