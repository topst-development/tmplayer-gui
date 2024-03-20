/****************************************************************************************
 *   FileName    : TMPlayerGUIDBusManager.cpp
 *   Description : TMPlayerGUIDBusManager.cpp
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
No warranty is made, express or implied, regarding the information¡¯s accuracy,
completeness, or performance.
In no event shall Telechips be liable for any claim, damages or other liability arising from,
out of or in connection with this source code or the use in the source code.
This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement
between Telechips and Company.
*
****************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dbus/dbus.h>
#include "DBusMsgDef.h"
#include "TCDBusRawAPI.h"
#include "TMPlayerGUIIFManager.h"
#include "TMPlayerGUIDBusManager.h"

extern int g_debug;
#define TM_PLAYER_DBUS_MANAGER_PRINTF(format, arg...) \
	if (g_debug) \
	{ \
		fprintf(stderr, "[TMP DBUS MANAGER] %s: " format "", __FUNCTION__, ##arg); \
	}

static DBusMsgErrorCode OnReceivedDBusSignal(DBusMessage *message, const char *interface);
static void LauncherSignalDBusProcess(unsigned int id, DBusMessage *message);
static void TCPlayerSignalDBusProcess(unsigned int id, DBusMessage *message);

static void OnDBusInitialized(void);

void InitializeTMPlayerDBusManager(void)
{
	SetCallBackFunctions(OnReceivedDBusSignal, NULL);
	AddSignalInterface(LAUNCHER_EVENT_INTERFACE);
	AddSignalInterface(TMPLAYER_EVENT_INTERFACE);
	InitializeRawDBusConnection("TM PLAYER DBUS");
	SetDBusInitCallBackFunctions(OnDBusInitialized);
}

void ReleaseTMPlayerDBusManager(void)
{
	ReleaseRawDBusConnection();
}

void SendDBusActiveApplication(int app, bool active)
{
	TM_PLAYER_DBUS_MANAGER_PRINTF("APPLICATION(%d), ACTIVE(%s)\n",
										 app, (active != 0) ? "TRUE" : "FALSE");
	DBusMessage *message;
	dbus_bool_t active_d;

	active_d = active ? TRUE : FALSE;


	message = CreateDBusMsgMethodCall(LAUNCHER_PROCESS_DBUS_NAME, LAUNCHER_PROCESS_OBJECT_PATH,
									  LAUNCHER_EVENT_INTERFACE,
									  METHOD_LAUNCHER_ACTIVE_APPLICATION,
									  DBUS_TYPE_INT32, &app,
									  DBUS_TYPE_BOOLEAN, &active_d,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

int SendDBusSetAppID(int appID)
{
	DBusMessage *message;
	int ret = -1;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_SET_APP_ID,
									  DBUS_TYPE_INT32, &appID,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		DBusPendingCall* pending = NULL;
		if (SendDBusMessage(message, &pending))
		{
			if(pending != NULL)
			{
				if(GetArgumentFromDBusPendingCall(pending,
												DBUS_TYPE_INT32, &ret,
												DBUS_TYPE_INVALID))
				{
					TM_PLAYER_DBUS_MANAGER_PRINTF("GetArgmentFromDBusPendingCall ret : %d \n", ret);
				}
				else
				{
					TM_PLAYER_DBUS_MANAGER_PRINTF("GetArgmentFromDBusPendingCall return error\n");
				}
				dbus_pending_call_unref(pending);
			}
		}
		else
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}
		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}

	return ret;
}

void SendDBusChangeContentType(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_CHANGE_CONTENT_TYPE,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusChangeNextDevice(bool play)
{
	DBusMessage *message;
	dbus_bool_t play_d;

	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	play_d = play ? TRUE : FALSE;
	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_CHANGE_NEXT_DEVICE,
									  DBUS_TYPE_BOOLEAN, &play_d,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusChangeDevice(unsigned char device, bool play)
{
	DBusMessage *message;
	dbus_bool_t play_d;

	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	play_d = play ? TRUE : FALSE;
	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_CHANGE_DEVICE,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_BOOLEAN, &play_d,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusSetBrowsingMode(unsigned int device, unsigned char content, bool meta)
{
	DBusMessage *message;
	dbus_bool_t meta_d;

	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	meta_d = meta ? TRUE : FALSE;
	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_SET_BROWSING_MODE,
									  DBUS_TYPE_UINT32, &device,
									  DBUS_TYPE_BYTE, &content,
									  DBUS_TYPE_BOOLEAN, &meta_d,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusChangeRepeatMode(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_CHANGE_REPEATMODE,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusChangeShuffleMode(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_CHANGE_SHUFFLEMODE,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusPlayStart(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_START,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusPlayStop(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_STOP,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusPlayResume(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_RESUME,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusPlayPause(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_PAUSE,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackUp(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_UP,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackDown(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_DOWN,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackMove(int number)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_MOVE,
									  DBUS_TYPE_INT32, &number,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackSeek(unsigned char hour, unsigned char min, unsigned char sec)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_SEEK,
									  DBUS_TYPE_BYTE, &hour,
									  DBUS_TYPE_BYTE, &min,
									  DBUS_TYPE_BYTE, &sec,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackForward(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_FORWARD,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackRewind(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_REWIND,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusTrackFFREWEnd(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_TRACK_FFREWEND,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusUpdateVideoDisplayInfo(int x, int y, int width, int height)
{
	DBusMessage *message;

	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_UPDATE_DISPLAY_INFO,
									  DBUS_TYPE_INT32, &x,
									  DBUS_TYPE_INT32, &y,
									  DBUS_TYPE_INT32, &width,
									  DBUS_TYPE_INT32, &height,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusUpdateVideoExtDisplayInfo(int x, int y, int width, int height)
{
	DBusMessage *message;

	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_UPDATE_EXT_DISPLAY_INFO,
									  DBUS_TYPE_INT32, &x,
									  DBUS_TYPE_INT32, &y,
									  DBUS_TYPE_INT32, &width,
									  DBUS_TYPE_INT32, &height,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusRequestFileList(unsigned char device, unsigned char content, int number, unsigned int count)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_REQUEST_FILELIST,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_BYTE, &content,
									  DBUS_TYPE_INT32, &number,
									  DBUS_TYPE_UINT32, &count,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusMetaBrowsingStart(unsigned char device, unsigned int count)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_META_BROWSING_START,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_UINT32, &count,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusMetaBrowsingRequest(unsigned char device, int index)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_META_BROWSING_REQUEST,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_INT32, &index,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusMetaBrowsingSelect(unsigned char device, int index)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_META_BROWSING_SELECT,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_INT32, &index,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusMetaBrowsingHome(unsigned char device, unsigned int count)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_META_BROWSING_HOME,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_UINT32, &count,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusMetaBrowsingUndo(unsigned char device)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_META_BROWSING_UNDO,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusMetaBrowsingEnd(unsigned char device, int index)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_META_BROWSING_END,
									  DBUS_TYPE_BYTE, &device,
									  DBUS_TYPE_INT32, &index,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}

		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusRequestDisplay(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_REQUEST_DISPLAY,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}
		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusGoLauncherHome(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_GO_LAUNCHER_HOME,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}
		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusGoLauncherBack(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_GO_LAUNCHER_BACK,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}
		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusGoAVOnOff(void)
{
	DBusMessage *message;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_GO_AV_ON_OFF,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}
		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

void SendDBusNotifyDisplayDone(bool on)
{
	DBusMessage *message;
	dbus_bool_t on_d;

	on_d = on ? TRUE : FALSE;
	TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	message = CreateDBusMsgMethodCall(TMPLAYER_PROCESS_DBUS_NAME, TMPLAYER_PROCESS_OBJECT_PATH,
									  TMPLAYER_EVENT_INTERFACE,
									  METHOD_TMPLAYER_NOTIFY_DISPLAY_DONE,
									  DBUS_TYPE_BOOLEAN, &on_d,
									  DBUS_TYPE_INVALID);
	if (message != NULL)
	{
		if (!SendDBusMessage(message, NULL))
		{
			fprintf(stderr, "%s: SendDBusMessage failed\n", __FUNCTION__);
		}
		dbus_message_unref(message);
	}
	else
	{
		fprintf(stderr, "%s: CreateDBusMsgMethodCall failed\n", __FUNCTION__);
	}
}

static DBusMsgErrorCode OnReceivedDBusSignal(DBusMessage *message, const char *interface)
{
	DBusMsgErrorCode error = ErrorCodeNoError;
	//TM_PLAYER_DBUS_MANAGER_PRINTF("\n");

	if (message != NULL &&
		interface != NULL)
	{
		unsigned int index;
		unsigned int stop = 0;

		if (strcmp(interface, LAUNCHER_EVENT_INTERFACE) == 0)
		{
			for (index = SignalLauncherKeyPressed; index < TotalSignalLauncherEvents && !stop; index++)
			{
				if (dbus_message_is_signal(message,
										   LAUNCHER_EVENT_INTERFACE,
										   g_signalLauncherEventNames[index]))
				{
					LauncherSignalDBusProcess(index, message);
					stop = 1;
				}
			}
		}
		else if (strcmp(interface, TMPLAYER_EVENT_INTERFACE) == 0)
		{
			for (index = SignalTCPlayerPlayFailed; index < TotalSignalTCPlayerEvent && !stop; index++)
			{
				if (dbus_message_is_signal(message,
										   TMPLAYER_EVENT_INTERFACE,
										   g_signalTCPlayerEventNames[index]))
				{
					TCPlayerSignalDBusProcess(index, message);
					stop = 1;
				}
			}
		}

		if (!stop)
		{
			error = ErrorCodeUnknown;
		}
	}

	return error;
}

static void LauncherSignalDBusProcess(unsigned int id, DBusMessage *message)
{
	if (id < TotalSignalLauncherEvents)
	{
		TM_PLAYER_DBUS_MANAGER_PRINTF("RECEIVED SIGNAL[%s(%d)]\n",
									  g_signalLauncherEventNames[id], id);

		if (message != NULL)
		{
			int value = -1;
			if (GetArgumentFromDBusMessage(message,
										   DBUS_TYPE_INT32, &value,
										   DBUS_TYPE_INVALID))
			{
				switch (id)
				{
					case SignalLauncherKeyPressed:
						TM_PLAYER_GUI_IF_MANAGER->EmitKeyboardPressedEvent(value);
						break;
					case SignalLauncherKeyLongPressed:
						TM_PLAYER_GUI_IF_MANAGER->EmitKeyboardLongPressedEvent(value);
						break;
					case SignalLauncherKeyLongLongPressed:
						TM_PLAYER_GUI_IF_MANAGER->EmitKeyboardLongLongPressedEvent(value);
						break;
					case SignalLauncherKeyReleased:
						TM_PLAYER_GUI_IF_MANAGER->EmitKeyboardReleasedEvent(value);
						break;
					case SignalLauncherKeyClicked:
						TM_PLAYER_GUI_IF_MANAGER->EmitKeyboardClickedEvent(value);
						break;
					default:
						fprintf(stderr, "%s: unknown signal id(%d)\n", __FUNCTION__, id);
						break;
				}
			}
		}
		else
		{
			fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __FUNCTION__);
		}
	}
	else
	{
		TM_PLAYER_DBUS_MANAGER_PRINTF("RECEIVED SIGNAL[%d]\n",
									  id);
	}
}

static void TCPlayerSignalDBusProcess(unsigned int id, DBusMessage *message)
{
	if (id < TotalSignalTCPlayerEvent)
	{
		if ((id != SignalTCPlayerPlayTimeChanged) && (id != SignalTCPlayerTotalTimeChanged))
		{
			TM_PLAYER_DBUS_MANAGER_PRINTF("RECEIVED SIGNAL[%s(%d)]\n",
										  g_signalTCPlayerEventNames[id], id);
		}


		if (message != NULL)
		{
			switch (id)
			{
				case SignalTCPlayerPlayStateChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitPlayStateChanged(message);
					break;
				case SignalTCPlayerPlayFailed:
					TM_PLAYER_GUI_IF_MANAGER->EmitPlayFailed(message);
					break;
				case SignalTCPlayerPlayTimeChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitPlayTimeChanged(message);
					break;
				case SignalTCPlayerRepeatModeChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitRepeatModeChange(message);
					break;
				case SignalTCPlayerShuffleModeChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitShuffleModeChange(message);
					break;
				case SignalTCPlayerTotalTimeChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitTotalTimeChange(message);
					break;
				case SignalTCPlayerSeekCompleted:
					TM_PLAYER_GUI_IF_MANAGER->EmitSeekComplete(message);
					break;
				case SignalTCPlayerID3Information:
					TM_PLAYER_GUI_IF_MANAGER->EmitID3Information(message);
					break;
				case SignalTCPlayerAlbumArtKey:
					TM_PLAYER_GUI_IF_MANAGER->EmitAlbumArtKey(message);
					break;
				case SignalTCPlayerAlbumArtCompleted:
					TM_PLAYER_GUI_IF_MANAGER->EmitAlbumArtCompleted(message);
					break;
				case SignalTCPlayerNoDevice:
					TM_PLAYER_GUI_IF_MANAGER->EmitNoDevice(message);
					break;
				case SignalTCPlayerContentChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitContentChanged(message);
					break;
				case SignalTCPlayerDeviceChagned:
					TM_PLAYER_GUI_IF_MANAGER->EmitDeviceChanged(message);
					break;
				case SignalTCPlayerDeviceConnected:
					TM_PLAYER_GUI_IF_MANAGER->EmitDeviceConnected(message);
					break;
				case SignalTCPlayerDeviceDisConnected:
					TM_PLAYER_GUI_IF_MANAGER->EmitDeviceDisconnected(message);
					break;
				case SignalTCPlayerDeviceReady:
					TM_PLAYER_GUI_IF_MANAGER->EmitDeviceMounted(message);
					break;
				case SignalTCPlayerDeviceMountStop:
					TM_PLAYER_GUI_IF_MANAGER->EmitDeviceMountStop(message);
					break;
				case SignalTCPlayerNoSong:
					TM_PLAYER_GUI_IF_MANAGER->EmitNoSong(message);
					break;
				case SignalTCPlayerResetInormation:
					TM_PLAYER_GUI_IF_MANAGER->EmitResetInformation(message);
					break;
				case SignalTCPlayerFileNumberChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitFileNumberChange(message);
					break;
				case SignalTCPlayerFileNumberUpdated:
					TM_PLAYER_GUI_IF_MANAGER->EmitFileNumberUpdate(message);
					break;
				case SignalTCPlayerTotalNumberChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitTotalNumberChange(message);
					break;
				case SignalTCPlayerFileDBCompleted:
					TM_PLAYER_GUI_IF_MANAGER->EmitFileDBComplete(message);
					break;
				case SignalTCPlayerFileNameChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitFileNameChange(message);
					break;
				case SignalTCPlayerFolderNameChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitFolderNameChange(message);
					break;
				case SignalTCPlayerFileList:
					TM_PLAYER_GUI_IF_MANAGER->EmitFileList(message);
					break;
				case SignalTCPlayerFileListChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitFileListChanged(message);
					break;
				case SignalTCPlayerMetaCountChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitMetaCountChanged(message);
					break;
				case SignalTCPlayerMetaMenuChanged:
					TM_PLAYER_GUI_IF_MANAGER->EmitMetaMenuChanged(message);
					break;
				case SignalTCPlayerMetaAddInfo:
					TM_PLAYER_GUI_IF_MANAGER->EmitMetaAddInfo(message);
					break;
				case SignalTCPlayerMetaCreateTrackList:
					TM_PLAYER_GUI_IF_MANAGER->EmitMetaCreateTrackList(message);
					break;
				case SignalTCPlayerDisplayOn:
					TM_PLAYER_GUI_IF_MANAGER->EmitDisplayOn(message);
					break;
				case SignalTCPlayerDisplayOff:
					TM_PLAYER_GUI_IF_MANAGER->EmitDisplayOff(message);
					break;
				case SignalTMPlayerAlbumArtShmData:
					TM_PLAYER_GUI_IF_MANAGER->EmitAlbumArtShmData(message);
					break;
				case SignalTMPlayerSamplerate:
#ifdef TDD_ENABLE
					TM_PLAYER_GUI_IF_MANAGER->EmitSamplerate(message);
					break;
#endif
				default:
					fprintf(stderr, "%s: unknown signal id(%d)\n", __FUNCTION__, id);
					break;
			}
		}
	}
	else
	{
		TM_PLAYER_DBUS_MANAGER_PRINTF("RECEIVED SIGNAL[%d]\n",id);
	}
}

static void OnDBusInitialized(void)
{
	TM_PLAYER_DBUS_MANAGER_PRINTF("D-Bus Initialize Done\n");
	TM_PLAYER_GUI_IF_MANAGER->EmitSetAppID();
}

