/****************************************************************************************
 *   FileName    : TMPlayerGUIIFManager.h
 *   Description : TMPlayerGUIIFManager.h
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

#ifndef TC_PLAYER_IF_MANAGER_H
#define TC_PLAYER_IF_MANAGER_H

#include <dbus/dbus.h>
#include <QObject>
#include <QThread>
#include "MediaPlayerType.h"
#include "DBusMsgDef.h"

class TMPlayerGUIIFManager : public QThread
{
	Q_OBJECT

	public:
		TMPlayerGUIIFManager(QObject *parent = 0);
		virtual ~TMPlayerGUIIFManager();
		int Initilaize(void);
		void Release(void);

		void EmitDisplayOn(DBusMessage *message);
		void EmitDisplayOff(DBusMessage *message);
		void EmitPlayStateChanged(DBusMessage *message);
		void EmitPlayFailed(DBusMessage *message);
		void EmitPlayTimeChanged(DBusMessage *message);
		void EmitRepeatModeChange(DBusMessage *message);
		void EmitShuffleModeChange(DBusMessage *message);
		void EmitTotalTimeChange(DBusMessage *message);
		void EmitSeekComplete(DBusMessage *message);
		void EmitID3Information(DBusMessage *message);
		void EmitAlbumArtKey(DBusMessage *message);
		void EmitAlbumArtCompleted(DBusMessage *message);
		void EmitAlbumArtShmData(DBusMessage *message);
		void EmitNoDevice(DBusMessage *message);
		void EmitContentChanged(DBusMessage *message);
		void EmitDeviceChanged(DBusMessage *message);
		void EmitDeviceConnected(DBusMessage *message);
		void EmitDeviceDisconnected(DBusMessage *message);
		void EmitDeviceMounted(DBusMessage *message);
		void EmitDeviceMountStop(DBusMessage *message);
		void EmitReadingError(DBusMessage *message);
		void EmitNoSong(DBusMessage *message);
		void EmitResetInformation(DBusMessage *message);
		void EmitFileNumberChange(DBusMessage *message);
		void EmitFileNumberUpdate(DBusMessage *message);
		void EmitTotalNumberChange(DBusMessage *message);
		void EmitFileDBComplete(DBusMessage *message);
		void EmitFileNameChange(DBusMessage *message);
		void EmitFolderNameChange(DBusMessage *message);
		void EmitFileList(DBusMessage *message);
		void EmitFileListChanged(DBusMessage *message);

		void EmitMetaCountChanged(DBusMessage *message);
		void EmitMetaMenuChanged(DBusMessage *message);
		void EmitMetaAddInfo(DBusMessage *message);
		void EmitMetaCreateTrackList(DBusMessage *message);

		void EmitKeyboardPressedEvent(int key);
		void EmitKeyboardLongPressedEvent(int key);
		void EmitKeyboardLongLongPressedEvent(int key);
		void EmitKeyboardReleasedEvent(int key);
		void EmitKeyboardClickedEvent(int key);

		void EmitUpdateExtDisplay(int x, int y, int width, int height);

		void EmitSetAppID(void);

	private:

	private slots:
// TC Player Interface
		void OnSetAppID(void);
		void OnNotifyDisplayDone(bool on);
		void OnGoLauncherHome(void);
		void OnGoLauncherBack(void);
		void OnGoAVOnOff(void);
		void OnRequestDisplay(void);
		void OnChangeContentType(void);
		void OnChangeNextDevice(bool play);
		void OnChangeDevice(unsigned char device, bool play);
		void OnSetBrowsingMode(unsigned int device, unsigned char content, bool meta);
		void OnChangeRepeatMode(void);
		void OnChangeShuffleMode(void);
		void OnPlayStart(void);
		void OnPlayStop(void);
		void OnPlayResume(void);
		void OnPlayPause(void);
		void OnTrackUp(void);
		void OnTrackDown(void);
		void OnTrackMove(int number);
		void OnTrackSeek(unsigned char hour, unsigned char min, unsigned char sec);
		void OnTrackForward(void);
		void OnTrackRewind(void);
		void OnTrackTurboForward(void);
		void OnTrackTurboRewind(void);
		void OnTrackFFREWEnd(void);
		void OnUpdateOverlayInfo(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		void OnRequestFileList(int device, unsigned char content, int number, unsigned int count);
		void OnMetaBrowsingStart(unsigned int device, unsigned int count);
		void OnMetaBrowsingRequest(unsigned int device, int index);
		void OnMetaBrowsingSelect(unsigned int device, int index);
		void OnMetaBrowsingHome(unsigned int device, unsigned int count);
		void OnMetaBrowsingUndo(unsigned int device);
		void OnMetaBrowsingEnd(unsigned int device, int index);
		void OnAlbumArtComplete(void);

// Launcher Interface
		void OnActiveApplication(int app, bool active);

	signals:
		void InitInformation(void);
		void FailInit(void);
		void PlayStateChanged(int status);
		void PlayFailed(int code);
		void PlayTimeChange(unsigned char hour, unsigned char min, unsigned char sec);
		void RepeatModeChange(unsigned int mode);
		void ShuffleModeChange(unsigned int mode);
		void TotalTimeChange(unsigned char hour, unsigned char min, unsigned char sec);
		void SeekComplete(void);
		void ID3Information(int category, QString);
		void AlbumArtKey(int key, unsigned int);
		void AlbumArtCompleted(int number, unsigned int length);
		void AlbumArtShmData(const char *uri, unsigned int length);
		void NoDevice(void);
		void ContentChanged(int content);
		void DeviceChanged(int device, int content);
		void DeviceConnected(int device);
		void DeviceDisconnected(int device);
		void DeviceMounted(int device);
		void DeviceMountStop(int device);
		void NoSong(int device);
		void ResetInformation();
		void FileNumberChange(int number);
		void FileNumberUpdate(int device, int content, int number, int totalNumber);
		void TotalNumberChange(int device, int content, int number);
		void FileDBComplete(int device);
		void FileNameChange(QString name);
		void FolderNameChange(QString name);
		void FileList(int device, int content, int number, QString name);
		void FileListChanged(int device, int content, int number,int totalNumber);

		void DisplayOn();
		void DisplayOff();

		void MetaCountChanged(int device, int count);
		void MetaMenuChanged(int device, int category, QString menu);
		void MetaAddInfo(int device, int index, QString name, unsigned int type);
		void MetaCreateTrackList(int device, int count);

		void KeyboardPressed(int key);
		void KeyboardLongPressed(int key);
		void KeyboardLongLongPressed(int key);
		void KeyboardReleased(int key);
		void KeyboardClicked(int key);

		void SetAppID();
};

TMPlayerGUIIFManager *GetTMPlayerGUIIFManager();
#define TM_PLAYER_GUI_IF_MANAGER	(GetTMPlayerGUIIFManager())

#endif // TC_PLAYER_IF_MANAGER_H
