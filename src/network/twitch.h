#ifndef _TWITCH_H_
#define _TWITCH_H_

#include "../common.h"

extern bool gTwitchEnable;

void twitch_update();
void twitch_login(utf8string channel, utf8string password);
void twitch_logout();
void twitch_setWindow(rct_window *w);

#endif