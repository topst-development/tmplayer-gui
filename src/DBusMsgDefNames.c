/****************************************************************************************
 *   FileName    : DBusMsgDefNames.c
 *   Description : DBusMsgDefNames.c
 ****************************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips Inc.
 *   All rights reserved

This source code contains confidential information of Telechips.
Any unauthorized use without a written permission of Telechips including not limited
to re-distribution in source or binary form is strictly prohibited.
This source code is provided "AS IS" and nothing contained in this source code
shall constitute any express or implied warranty of any kind, including without limitation,
any warranty of merchantability, fitness for a particular purpose or non-infringement of any patent,
copyright or other third party intellectual property right.
No warranty is made, express or implied, regarding the information��s accuracy,
completeness, or performance.
In no event shall Telechips be liable for any claim, damages or other liability arising from,
out of or in connection with this source code or the use in the source code.
This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement
between Telechips and Company.
*
****************************************************************************************/


#include "DBusMsgDef.h"

const char *g_signalLauncherEventNames[TotalSignalLauncherEvents] = {
	SIGNAL_LAUNCHER_KEY_PRESSED,
	SIGNAL_LAUNCHER_KEY_LONG_PRESSED,
	SIGNAL_LAUNCHER_KEY_LONG_LONG_PRESSED,
	SIGNAL_LAUNCHER_KEY_RELEASED,
	SIGNAL_LAUNCHER_KEY_CLICKED
};

const char *g_methodLauncherEventNames[TotalMethodLauncherEvents] = {
	METHOD_LAUNCHER_ACTIVE_APPLICATION,
	METHOD_LAUNCHER_START_APPLICATIONS
};

const char *g_signalTCPlayerEventNames[TotalSignalTCPlayerEvent] = {
	SIGNAL_TMPLAYER_PLAY_FAILED,
	SIGNAL_TMPLAYER_PLAYSTATE_CHANGED,
	SIGNAL_TMPLAYER_PLAYTIME_CHANGED,
	SIGNAL_TMPLAYER_REPEATMODE_CHANGED,
	SIGNAL_TMPLAYER_SHUFFLEMODE_CHANGED,
	SIGNAL_TMPLAYER_TOTALTIME_CHANGED,
	SIGNAL_TMPLAYER_SEEK_COMPLETED,
	SIGNAL_TMPLAYER_ID3_INFOMATION,
	SIGNAL_TMPLAYER_ALBUMART_KEY,
	SIGNAL_TMPLAYER_ALBUMART_COMPLETED,
	SIGNAL_TMPLAYER_NO_DEVICE,
	SIGNAL_TMPLAYER_CONTENT_CHANGED,
	SIGNAL_TMPLAYER_DEVICE_CHANGED,
	SIGNAL_TMPLAYER_DEVICE_CONNECTED,
	SIGNAL_TMPLAYER_DEVICE_DISCONNECTED,
	SIGNAL_TMPLAYER_DEVICE_READY,
	SIGNAL_TMPLAYER_DEVICE_MOUNT_STOP,
	SIGNAL_TMPLAYER_NO_SONG,
	SIGNAL_TMPLAYER_RESET_INFORMATION,
	SIGNAL_TMPLAYER_FILENUMBER_CHANGED,
	SIGNAL_TMPLAYER_FILENUMBER_UPDATED,
	SIGNAL_TMPLAYER_TOTALNUMBER_CHAGNED,
	SIGNAL_TMPLAYER_FILE_DB_COMPLETED,
	SIGNAL_TMPLAYER_FILENAME_CHANGED,
	SIGNAL_TMPLAYER_FOLDERNAME_CHANGED,
	SIGNAL_TMPLAYER_FILELIST,
	SIGNAL_TMPLAYER_FILELIST_CHANGED,
	SIGNAL_TMPLAYER_META_COUNT_CHANGED,
	SIGNAL_TMPLAYER_META_MENU_CHANGED,
	SIGNAL_TMPLAYER_META_ADD_INFO,
	SIGNAL_TMPLAYER_META_CREATE_TRACKLIST,
	SIGNAL_TMPLAYER_DISPLAY_ON,
	SIGNAL_TMPLAYER_DISPLAY_OFF,
	SIGNAL_TMPLAYER_ALBUMART_SHMDATA,
	SIGNAL_TMPLAYER_SAMPLERATE
};

const char *g_methodTCPlayerEventNames[TotalMethodTCPlayerEvent] = {
	METHOD_TMPLAYER_SET_APP_ID,
	METHOD_TMPLAYER_CONNECT_DEBUG,
	METHOD_TMPLAYER_DB_DEBUG,
	METHOD_TMPLAYER_CHANGE_CONTENT_TYPE,
	METHOD_TMPLAYER_CHANGE_NEXT_DEVICE,
	METHOD_TMPLAYER_CHANGE_DEVICE,
	METHOD_TMPLAYER_SET_BROWSING_MODE,
	METHOD_TMPLAYER_CHANGE_REPEATMODE,
	METHOD_TMPLAYER_CHANGE_SHUFFLEMODE,
	METHOD_TMPLAYER_TRACK_START,
	METHOD_TMPLAYER_TRACK_STOP,
	METHOD_TMPLAYER_TRACK_RESUME,
	METHOD_TMPLAYER_TRACK_PAUSE,
	METHOD_TMPLAYER_TRACK_UP,
	METHOD_TMPLAYER_TRACK_DOWN,
	METHOD_TMPLAYER_TRACK_MOVE,
	METHOD_TMPLAYER_TRACK_SEEK,
	METHOD_TMPLAYER_TRACK_FORWARD,
	METHOD_TMPLAYER_TRACK_REWIND,
	METHOD_TMPLAYER_TRACK_FFREWEND,
	METHOD_TMPLAYER_UPDATE_DISPLAY_INFO,
	METHOD_TMPLAYER_UPDATE_EXT_DISPLAY_INFO,
	METHOD_TMPLAYER_REQUEST_ALBUMART_KEY,
	METHOD_TMPLAYER_REQUEST_FILELIST,
	METHOD_TMPLAYER_META_BROWSING_START,
	METHOD_TMPLAYER_META_BROWSING_REQUEST,
	METHOD_TMPLAYER_META_BROWSING_SELECT,
	METHOD_TMPLAYER_META_BROWSING_HOME,
	METHOD_TMPLAYER_META_BROWSING_UNDO,
	METHOD_TMPLAYER_META_BROWSING_END,
	METHOD_TMPLAYER_REQUEST_DISPLAY,
	METHOD_TMPLAYER_GO_LAUNCHER_HOME,
	METHOD_TMPLAYER_GO_LAUNCHER_BACK,
	METHOD_TMPLAYER_GO_AV_ON_OFF,
	METHOD_TMPLAYER_NOTIFY_DISPLAY_DONE
};

const char *g_deviceSourceNames[TotalDeviceSources] = {
	"NAND",
	"USB1",
	"USB2",
	"USB3",
	"SDMMC",
	"AUX",
	"iAP2",
	"BT"
};

const char *g_AVPlayStatusNames[TotalPlayStatus] = {
	"Play Status(Stop)",
	"Play Status(Playing)",
	"Play Status(Pause)",
	"Play Status(Fast Forward)",
	"Play Status(Fast Rewind)",
	"Play Status(Forward/Rewind End)"
	"Play Status(Turbo Fast Forward)",
	"Play Status(Turbo Fast Rewind)",
	"Play Status(NoStatement)",
};

/* End of file */
