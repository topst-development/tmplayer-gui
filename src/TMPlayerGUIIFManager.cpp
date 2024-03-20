/****************************************************************************************
 *   FileName    : TMPlayerGUIIFManager.cpp
 *   Description : TMPlayerGUIIFManager.cpp
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

#include <QTimer>
#include <QTextCodec>
#include <cstdlib>
#include <cstdio>
#include <pthread.h>
#include <string.h>
#include "TCInput.h"
#include "TCDBusRawAPI.h"
#include "TMPlayerGUIDBusManager.h"
#include "TMPlayerGUIIFManager.h"

extern int g_debug;
extern int g_appID;

extern QTextCodec *g_codecUtf16;
extern QTextCodec *g_codecUtf8;

static int s_init_count = 0;

#define MASS_PLAYER_DBUS_MANAGER_PRINTF(format, arg...) \
	if (g_debug) \
	{ \
		fprintf(stderr, "[TMP IF MANAGER] %s: " format "", __FUNCTION__, ##arg); \
	}

extern QString GetNameString(const char *buffer, unsigned int length, bool utf16 = false);

TMPlayerGUIIFManager::TMPlayerGUIIFManager(QObject *parent) :
		QThread(parent)
{
}

TMPlayerGUIIFManager::~TMPlayerGUIIFManager()
{
	this->exit();
	this->wait();
}

int TMPlayerGUIIFManager::Initilaize(void)
{
	// to do: check initialize result
	int ret = 1;

	connect(this, SIGNAL(SetAppID()), this, SLOT(OnSetAppID()));

	this->start();
	InitializeTMPlayerDBusManager();

	return ret;
}

void TMPlayerGUIIFManager::Release(void)
{
	ReleaseTMPlayerDBusManager();
}

void TMPlayerGUIIFManager::EmitDisplayOn(DBusMessage *message)
{
	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_INVALID))
	{
		emit DisplayOn();
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitDisplayOff(DBusMessage *message)
{
	emit DisplayOff();
	(void)message;
}

void TMPlayerGUIIFManager::EmitPlayStateChanged(DBusMessage *message)
{
	unsigned char status;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &status,
								   DBUS_TYPE_INVALID))
	{
		emit PlayStateChanged((int)status);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitPlayFailed(DBusMessage *message)
{
	int code;
	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_INT32, &code,
								   DBUS_TYPE_INVALID))
	{
		emit PlayFailed(code);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitPlayTimeChanged(DBusMessage *message)
{
	unsigned char hour;
	unsigned char min;
	unsigned char sec;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &hour,
								   DBUS_TYPE_BYTE, &min,
								   DBUS_TYPE_BYTE, &sec,
								   DBUS_TYPE_INVALID))
	{
		emit PlayTimeChange(hour, min, sec);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitRepeatModeChange(DBusMessage *message)
{
	unsigned int mode;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_UINT32, &mode,
								   DBUS_TYPE_INVALID))
	{
		emit RepeatModeChange(mode);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitShuffleModeChange(DBusMessage *message)
{
	unsigned int mode;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_UINT32, &mode,
								   DBUS_TYPE_INVALID))
	{
		emit ShuffleModeChange(mode);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitTotalTimeChange(DBusMessage *message)
{
	unsigned char hour;
	unsigned char min;
	unsigned char sec;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &hour,
								   DBUS_TYPE_BYTE, &min,
								   DBUS_TYPE_BYTE, &sec,
								   DBUS_TYPE_INVALID))
	{
		emit TotalTimeChange(hour, min, sec);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitSeekComplete(DBusMessage *message)
{
	emit SeekComplete();
	(void)message;
}

void TMPlayerGUIIFManager::EmitID3Information(DBusMessage *message)
{
	int category;
	char *info;

	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_INT32, &category,
								   DBUS_TYPE_STRING, &info,
								   DBUS_TYPE_INVALID))
	{
		emit ID3Information(category, GetNameString(info, strlen(info)));
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitAlbumArtKey(DBusMessage *message)
{
	int key;
	unsigned int size;
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_INT32, &key,
								   DBUS_TYPE_UINT32, &size,
								   DBUS_TYPE_INVALID))
	{
		emit AlbumArtKey(key, size);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitAlbumArtCompleted(DBusMessage *message)
{
	int number;
	unsigned int length;
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_INT32, &number,
								   DBUS_TYPE_UINT32, &length,
								   DBUS_TYPE_INVALID))
	{
		emit AlbumArtCompleted(number, length);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitAlbumArtShmData(DBusMessage *message)
{
	const char *uri;
	unsigned int size;
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_STRING, &uri,
								   DBUS_TYPE_UINT32, &size,
								   DBUS_TYPE_INVALID))
	{
		static char albumart[512];
		size_t len = strnlen(uri, 512);
		memcpy(albumart, uri, len);
		albumart[len] = (char)0x00;
		emit AlbumArtShmData(albumart, size);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitNoDevice(DBusMessage *message)
{
	emit NoDevice();
	(void)message;
}

void TMPlayerGUIIFManager::EmitContentChanged(DBusMessage *message)
{
	unsigned char contentType;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &contentType,
								   DBUS_TYPE_INVALID))
	{
		emit ContentChanged((int)contentType);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitDeviceChanged(DBusMessage *message)
{
	unsigned char device;
	unsigned char contentType;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_BYTE, &contentType,
								   DBUS_TYPE_INVALID))
	{
		emit DeviceChanged((int)device, (int)contentType);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitDeviceConnected(DBusMessage *message)
{
	unsigned char device;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INVALID))
	{
		emit DeviceConnected(device);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitDeviceDisconnected(DBusMessage *message)
{
	unsigned char device;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INVALID))
	{
		emit DeviceDisconnected(device);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitDeviceMounted(DBusMessage *message)
{
	unsigned char device;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INVALID))
	{
#if 1
		if (device < TotalDeviceSources)
#else
		if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
#endif
		{
			emit DeviceMounted(device);
		}
		else
		{
			fprintf(stderr, "%s: invalid device(%d)\n", __FUNCTION__, device);
		}
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitDeviceMountStop(DBusMessage *message)
{
	unsigned char device;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INVALID))
	{
		emit DeviceMountStop(device);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitNoSong(DBusMessage *message)
{
	unsigned char device;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INVALID))
	{
		emit NoSong(device);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitResetInformation(DBusMessage *message)
{
	emit ResetInformation();
	(void)message;
}

void TMPlayerGUIIFManager::EmitFileNumberChange(DBusMessage *message)
{
	int number;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_INT32, &number,
								   DBUS_TYPE_INVALID))
	{
		emit FileNumberChange(number);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitFileNumberUpdate(DBusMessage *message)
{
	unsigned char device;
	unsigned char content;
	int number;
	int totalNumber;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_BYTE, &content,
								   DBUS_TYPE_INT32, &number,
								   DBUS_TYPE_INT32, &totalNumber,
								   DBUS_TYPE_INVALID))
	{
		emit FileNumberUpdate(device,content,number, totalNumber);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitTotalNumberChange(DBusMessage *message)
{
	unsigned char device;
	unsigned char content;
	int totalNumber;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_BYTE, &content,
								   DBUS_TYPE_INT32, &totalNumber,
								   DBUS_TYPE_INVALID))
	{
		MASS_PLAYER_DBUS_MANAGER_PRINTF("DEVICE(%d), CONTENT(%d), TOTAL NUMBER(%u)\n",
										device, content, totalNumber);
		emit TotalNumberChange((int)device, (int)content, totalNumber);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitFileDBComplete(DBusMessage *message)
{
	unsigned char device;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INVALID))
	{
		emit FileDBComplete(device);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitFileNameChange(DBusMessage *message)
{	
	const char *name;
	unsigned int length;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_STRING, &name,
								   DBUS_TYPE_UINT32, &length,
								   DBUS_TYPE_INVALID))
	{
		QString fileName = GetNameString(name, length);
		emit FileNameChange(fileName);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitFolderNameChange(DBusMessage *message)
{
	const char *name;
	unsigned int length;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_STRING, &name,
								   DBUS_TYPE_UINT32, &length,
								   DBUS_TYPE_INVALID))
	{
		QString folderName = GetNameString(name,length);
		emit FolderNameChange(folderName);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

// FileList info
void TMPlayerGUIIFManager::EmitFileList(DBusMessage *message)
{
	unsigned char device;
	unsigned char content;
	int number;
	char **names;
	int count;
	QString name;

	if(GetArgumentFromDBusMessage(message,
								  DBUS_TYPE_BYTE, &device,
								  DBUS_TYPE_BYTE, &content,
								  DBUS_TYPE_INT32, &number,
								  DBUS_TYPE_ARRAY, DBUS_TYPE_STRING, &names, &count,
								  DBUS_TYPE_INVALID))
	{
		for (int i = 0; i < count; i++)
		{
			name = GetNameString(names[i], strlen(names[i]));
			emit FileList((int)device, (int)content, number, name);
			MASS_PLAYER_DBUS_MANAGER_PRINTF("%s: DEVICE(%d), CONTENT(%d), NUMBER(%d), NAME(%s)\n",
											__PRETTY_FUNCTION__,
											device, content, number, name.toLatin1().data())
			number++;
		}
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitFileListChanged(DBusMessage *message)
{
	unsigned char device;
	unsigned char content;
	int number;
	int totalNumber;

	if(GetArgumentFromDBusMessage(message,
								  DBUS_TYPE_BYTE, &device,
								  DBUS_TYPE_BYTE, &content,
								  DBUS_TYPE_INT32, &number,
								  DBUS_TYPE_INT32, &totalNumber,
								  DBUS_TYPE_INVALID))
	{
		emit FileListChanged((int)device, (int)content, number, totalNumber);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitMetaCountChanged(DBusMessage *message)
{
	unsigned char device;
	int count;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_UINT32, &count,
								   DBUS_TYPE_INVALID))
	{
		emit MetaCountChanged((int)device, count);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitMetaMenuChanged(DBusMessage *message)
{
	unsigned char device;
	unsigned char category;
	const char *menu;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_BYTE, &category,
								   DBUS_TYPE_STRING, &menu,
								   DBUS_TYPE_INVALID))
	{
		if (menu != NULL)
		{
			QString menuName = GetNameString(menu, strlen(menu) + 1);
			MASS_PLAYER_DBUS_MANAGER_PRINTF("DEVICE(%u), CATEGORY(%u), NAME(%s)\n",
											device, category, menu);
			emit MetaMenuChanged((int)device, (int)category, menuName);
		}
		else
		{
			fprintf(stderr, "%s: menu is null\n", __PRETTY_FUNCTION__);
		}
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitMetaAddInfo(DBusMessage *message)
{
	unsigned char device;
	int index;
	unsigned int type;
	const char *name;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_INT32, &index,
								   DBUS_TYPE_STRING, &name,
								   DBUS_TYPE_UINT32, &type,
								   DBUS_TYPE_INVALID))
	{
		if (name != NULL)
		{
			QString metaName = GetNameString(name, strlen(name) + 1);
			MASS_PLAYER_DBUS_MANAGER_PRINTF("DEVICE(%u), INDEX(%d), NAME(%s), TYPE(%d)\n",
											device, index, name, type);
			emit MetaAddInfo((int)device, index, metaName, type);
		}
		else
		{
			fprintf(stderr, "%s: name is null\n", __PRETTY_FUNCTION__);
		}
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitMetaCreateTrackList(DBusMessage *message)
{
	unsigned char device;
	unsigned int count;

	if (GetArgumentFromDBusMessage(message,
								   DBUS_TYPE_BYTE, &device,
								   DBUS_TYPE_UINT32, &count,
								   DBUS_TYPE_INVALID))
	{
		emit MetaCreateTrackList(device, count);
	}
	else
	{
		fprintf(stderr, "%s: GetArgumentFromDBusMessage failed\n", __PRETTY_FUNCTION__);
	}
}

void TMPlayerGUIIFManager::EmitKeyboardPressedEvent(int key)
{
	emit KeyboardPressed(key);
}

void TMPlayerGUIIFManager::EmitKeyboardLongPressedEvent(int key)
{
	emit KeyboardLongPressed(key);
}

void TMPlayerGUIIFManager::EmitKeyboardLongLongPressedEvent(int key)
{
	emit KeyboardLongLongPressed(key);
}

void TMPlayerGUIIFManager::EmitKeyboardReleasedEvent(int key)
{
	emit KeyboardReleased(key);
}

void TMPlayerGUIIFManager::EmitKeyboardClickedEvent(int key)
{
	emit KeyboardClicked(key);
}

void TMPlayerGUIIFManager::EmitUpdateExtDisplay(int x, int y, int width, int height)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("x(%u), y(%u), width(%u), height(%u)\n",
									  x, y, width, height);
	SendDBusUpdateVideoExtDisplayInfo(x, y, width, height);
}

void TMPlayerGUIIFManager::EmitSetAppID(void)
{
	emit SetAppID();
	disconnect(this, SIGNAL(SetAppID()), this, SLOT(OnSetAppID()));
}

void TMPlayerGUIIFManager::OnSetAppID()
{
	int ret;

	MASS_PLAYER_DBUS_MANAGER_PRINTF("%d\n", QThread::currentThreadId());
	ret = SendDBusSetAppID(g_appID);
	if (s_init_count < 10)
	{
		if(ret < 0)
		{
			QTimer::singleShot(1000, this, SLOT(OnSetAppID()));
			s_init_count++;
		}
		else
		{
			emit InitInformation();
		}
	}
	else
	{
		emit FailInit();
	}
}

void TMPlayerGUIIFManager::OnRequestDisplay()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusRequestDisplay();
}

void TMPlayerGUIIFManager::OnGoLauncherHome(void)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusGoLauncherHome();
}

void TMPlayerGUIIFManager::OnGoLauncherBack(void)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusGoLauncherBack();
}

void TMPlayerGUIIFManager::OnGoAVOnOff(void)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusGoAVOnOff();
}

void TMPlayerGUIIFManager::OnNotifyDisplayDone(bool on)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusNotifyDisplayDone(on);
}

void TMPlayerGUIIFManager::OnChangeContentType()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusChangeContentType();
}

void TMPlayerGUIIFManager::OnChangeNextDevice(bool play)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");

	SendDBusChangeNextDevice(play);
}

void TMPlayerGUIIFManager::OnChangeDevice(unsigned char device, bool play)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("device(%d)\n",
								  device);
	SendDBusChangeDevice(device, play);
}

void TMPlayerGUIIFManager::OnSetBrowsingMode(unsigned int device, unsigned char content, bool meta)
{
	SendDBusSetBrowsingMode(device, content, meta);
}

void TMPlayerGUIIFManager::OnChangeRepeatMode(void)
{
	SendDBusChangeRepeatMode();
}

void TMPlayerGUIIFManager::OnChangeShuffleMode(void)
{
	SendDBusChangeShuffleMode();
}

void TMPlayerGUIIFManager::OnPlayStart()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
	SendDBusPlayStart();
}

void TMPlayerGUIIFManager::OnPlayStop()
{
	SendDBusPlayStop();
}

void TMPlayerGUIIFManager::OnPlayResume()
{
	SendDBusPlayResume();
}

void TMPlayerGUIIFManager::OnPlayPause()
{
	SendDBusPlayPause();
}

void TMPlayerGUIIFManager::OnTrackUp()
{
	SendDBusTrackUp();
}

void TMPlayerGUIIFManager::OnTrackDown()
{
	SendDBusTrackDown();
}

void TMPlayerGUIIFManager::OnTrackMove(int number)
{
	SendDBusTrackMove(number);
}

void TMPlayerGUIIFManager::OnTrackSeek(unsigned char hour, unsigned char min, unsigned char sec)
{
	SendDBusTrackSeek(hour, min, sec);
}

void TMPlayerGUIIFManager::OnTrackForward()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
	SendDBusTrackForward();
}

void TMPlayerGUIIFManager::OnTrackRewind()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
	SendDBusTrackRewind();
}

void TMPlayerGUIIFManager::OnTrackTurboForward()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
}

void TMPlayerGUIIFManager::OnTrackTurboRewind()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
}

void TMPlayerGUIIFManager::OnTrackFFREWEnd()
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
	SendDBusTrackFFREWEnd();
}

void TMPlayerGUIIFManager::OnUpdateOverlayInfo(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("x(%u), y(%u), width(%u), height(%u)\n",
									  x, y, width, height);
	SendDBusUpdateVideoDisplayInfo(x, y, width, height);
}

void TMPlayerGUIIFManager::OnRequestFileList(int device, unsigned char content, int number, unsigned int count)
{
	MASS_PLAYER_DBUS_MANAGER_PRINTF("\n");
	SendDBusRequestFileList((unsigned char)device, content, number, count);
}

void TMPlayerGUIIFManager::OnMetaBrowsingStart(unsigned int device, unsigned int count)
{
	SendDBusMetaBrowsingStart((unsigned char)device, count);
}

void TMPlayerGUIIFManager::OnMetaBrowsingRequest(unsigned int device, int index)
{
	SendDBusMetaBrowsingRequest((unsigned char)device, index);
}

void TMPlayerGUIIFManager::OnMetaBrowsingSelect(unsigned int device, int index)
{
	SendDBusMetaBrowsingSelect((unsigned char)device, index);
}

void TMPlayerGUIIFManager::OnMetaBrowsingHome(unsigned int device, unsigned int count)
{
	SendDBusMetaBrowsingHome((unsigned char)device, count);
}

void TMPlayerGUIIFManager::OnMetaBrowsingUndo(unsigned int device)
{
	SendDBusMetaBrowsingUndo((unsigned char)device);
}

void TMPlayerGUIIFManager::OnMetaBrowsingEnd(unsigned int device, int index)
{
	SendDBusMetaBrowsingEnd((unsigned char)device, index);
}

void TMPlayerGUIIFManager::OnAlbumArtComplete()
{
}

void TMPlayerGUIIFManager::OnActiveApplication(int app, bool active)
{
	SendDBusActiveApplication(app, active);
}

static TMPlayerGUIIFManager _manager;
TMPlayerGUIIFManager *GetTMPlayerGUIIFManager()
{
	return &_manager;
}

QString GetNameString(const char *buffer, unsigned int length, bool utf16)
{
	QString text;
	if (utf16)
	{
		if (g_codecUtf16 != NULL)
		{
			text = g_codecUtf16->toUnicode(buffer, length);
		}
	}
	else
	{
		if (g_codecUtf8 != NULL)
		{
			text = g_codecUtf8->toUnicode(buffer, length);
		}
	}
	return text;
}
