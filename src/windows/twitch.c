/*****************************************************************************
* Copyright (c) 2014 Ted John
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "../addresses.h"
#include "../localisation/localisation.h"
#include "../sprites.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../config.h"
#include "../network/twitch.h"

char twitchUname[33];
char twitchPass[33];

enum window_twitch_WIDGET_IDX {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_TWITCH_AUTH,
	WIDX_TWITCH_DEAUTH,
	WIDX_TWITCH_UNAME,
	WIDX_TWITCH_PASS,
	//WIDX_FOLLOWER_PEEP_NAMES_CHECKBOX,
	//WIDX_FOLLOWER_PEEP_TRACKING_CHECKBOX,
	//WIDX_CHAT_PEEP_NAMES_CHECKBOX,
	//WIDX_CHAT_PEEP_TRACKING_CHECKBOX,
	//WIDX_NEWS_CHECKBOX,
	WINDOW_TWITCH_WIDGETS_SIZE
};

rct_widget window_twitch_widgets[] = {
	{ WWT_FRAME, 0, 0, 399, 0, 329, 0x0FFFFFFFF, STR_NONE },				// panel / background
	{ WWT_CAPTION, 0, 1, 398, 1, 14, STR_TWITCH_WINDOW_TITLE, STR_WINDOW_TITLE_TIP },	// title bar
	{ WWT_CLOSEBOX, 0, 387, 397, 2, 13, STR_CLOSE_X, STR_CLOSE_WINDOW_TIP },	// close x button
	{ WWT_DROPDOWN_BUTTON, 1, 100, 299, 53, 64, STR_TWITCH_AUTH, STR_TWITCH_AUTH_TIP },		// twitch auth
	{ WWT_DROPDOWN_BUTTON, 1, 100, 299, 53, 64, STR_TWITCH_DEAUTH, STR_TWITCH_DEAUTH_TIP },	// twitch deauth
	{ WWT_TEXT_BOX, 1, 100, 299, 68, 79, (uint32)twitchUname, STR_NONE },	// twitch username
	{ WWT_MASKED_TEXT_BOX, 1, 100, 299, 83, 94, (uint32)twitchPass, STR_NONE },	// twitch username
	//{ WWT_CHECKBOX, 2, 10, 299, 69, 80, STR_TWITCH_PEEP_FOLLOWERS, STR_TWITCH_PEEP_FOLLOWERS_TIP },	// Twitch name peeps by follows
	//{ WWT_CHECKBOX, 2, 10, 299, 84, 95, STR_TWITCH_FOLLOWERS_TRACK, STR_TWITCH_FOLLOWERS_TRACK_TIP },	// Twitch information on for follows
	//{ WWT_CHECKBOX, 2, 10, 299, 99, 110, STR_TWITCH_PEEP_CHAT, STR_TWITCH_PEEP_CHAT_TIP },			// Twitch name peeps by chat
	//{ WWT_CHECKBOX, 2, 10, 299, 114, 125, STR_TWITCH_CHAT_TRACK, STR_TWITCH_CHAT_TRACK_TIP },		// Twitch information on for chat
	//{ WWT_CHECKBOX, 2, 10, 299, 129, 140, STR_TWITCH_CHAT_NEWS, STR_TWITCH_CHAT_NEWS_TIP },			// Twitch chat !news as notifications in game
	{ WIDGETS_END },
};

static void window_twitch_emptysub() { }
static void window_twitch_mouseup();
static void window_twitch_paint();
static void window_twitch_invalidate();
static void window_twitch_text_input();
static void window_twitch_update(rct_window *w);

static void* window_twitch_events[] = {
	window_twitch_emptysub,
	window_twitch_mouseup,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_update,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_text_input,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_emptysub,
	window_twitch_invalidate,
	window_twitch_paint,
	window_twitch_emptysub
};

utf8string password;
bool loggingIn = false;

/**
*
*  rct2: 0x0066D2AC
*/
void window_twitch_open()
{
	rct_window* window;

	// Check if window is already open
	window = window_bring_to_front_by_class(WC_TWITCH);
	if (window != NULL)
		return;

	window = window_create_centred(
		400,
		330,
		(uint32*)window_twitch_events,
		WC_TWITCH,
		0
		);
	window->widgets = window_twitch_widgets;
	window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_TWITCH_AUTH) | (1 << WIDX_TWITCH_DEAUTH) | (1 << WIDX_TWITCH_UNAME) | (1 << WIDX_TWITCH_PASS);

	window_init_scroll_widgets(window);
	window->colours[0] = 7;
	window->colours[1] = 7;
	window->colours[2] = 7;

	twitch_setWindow(window);
}

/**
*
*  rct2: 0x0066D4D5
*/
static void window_twitch_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

    password = "";

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TWITCH_AUTH:
		window_multi_text_input_raw_open(w, widgetIndex, STR_TWITCH_AUTH, STR_TWITCH_NAME_DESC, gConfigTwitch.channel, 32, false, password, 32, true);
		break;
	case WIDX_TWITCH_DEAUTH:
		twitch_logout();
		break;
	case WIDX_TWITCH_UNAME:
		window_start_textbox(w, widgetIndex, 1170, (uint32)twitchUname, 32);
		break;
	case WIDX_TWITCH_PASS:
		window_start_textbox(w, widgetIndex, 1170, (uint32)twitchPass, 32);
		break;
	}
}

/**
*
*  rct2: 0x0066D321
*/
static void window_twitch_paint()
{
	int x, y;
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);

	x = w->x + 180;
	y = w->y + 17;

	gfx_draw_sprite(dpi, SPR_G2_TAB_TWITCH, x, y, 0);

	x = w->x + 300;
	y = w->y + 230;

	gfx_draw_sprite(dpi, SPR_G2_PROMO_UL, x, y, 0);

}

static void window_twitch_text_input(){
	short widgetIndex;
	rct_window *w;
	uint8 result;
	char* text;

	window_textinput_get_registers(w, widgetIndex, result, text);
	if (result == 0)
	{
		return;
	}

	log_info("%s", text);

	if (widgetIndex == WIDX_TWITCH_UNAME) {
		strcpy(twitchUname, text);
	} else if (widgetIndex == WIDX_TWITCH_PASS) {
		strcpy(twitchPass, text);
	}

	char *userName = multi_text_input;
	char *password = multi_text_input2;

	if (userName != NULL && userName[0] != 0 && password != NULL && password[0] != 0) {
		loggingIn = true;
		window_twitch_invalidate();
		log_info("Logging into twitch");
		twitch_login(userName, password);
	}
}

static void window_twitch_update(rct_window *w)
{
	if (gCurrentTextBox.window.classification == w->classification &&
		gCurrentTextBox.window.number == w->number) {
		window_update_textbox_caret();
		widget_invalidate(w, WIDX_TWITCH_UNAME);
		widget_invalidate(w, WIDX_TWITCH_PASS);
	}
}

static void window_twitch_invalidate()
{
	rct_window *w;

	window_get_register(w);

	if (gConfigTwitch.token == NULL || gConfigTwitch.token[0] == 0 && !loggingIn) {
		loggingIn = false;
		window_twitch_widgets[WIDX_TWITCH_DEAUTH].type = WWT_EMPTY;
		window_twitch_widgets[WIDX_TWITCH_AUTH].type = WWT_DROPDOWN_BUTTON;
		platform_set_cursor(0);
	} else if ((gConfigTwitch.token == NULL || gConfigTwitch.token[0] == 0) && loggingIn) {
		window_twitch_widgets[WIDX_TWITCH_DEAUTH].type = WWT_EMPTY;
		window_twitch_widgets[WIDX_TWITCH_AUTH].type = WWT_EMPTY;
		platform_set_cursor(5);
	} else {
		loggingIn = false;
		window_twitch_widgets[WIDX_TWITCH_DEAUTH].type = WWT_DROPDOWN_BUTTON;
		window_twitch_widgets[WIDX_TWITCH_AUTH].type = WWT_EMPTY;
		platform_set_cursor(0);
	}

	/*for (i = WIDX_TWITCH_AUTH; i < WINDOW_TWITCH_WIDGETS_SIZE; i++) {
		window_twitch_widgets[i].type = WWT_EMPTY;
	}*/
}
