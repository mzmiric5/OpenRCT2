#ifndef _TWITCH_H_
#define _TWITCH_H_

#include "../common.h"

extern bool gTwitchEnable;

void twitch_update();
void twitch_login();
void twitch_logout();

#endif