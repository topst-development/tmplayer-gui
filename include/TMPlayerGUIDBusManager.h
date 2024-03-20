/****************************************************************************************
 *   FileName    : TMPlayerGUIDBusManager.h
 *   Description : TMPlayerGUIDBusManager.h
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

#ifndef TC_PLAYER_DBUS_MANAGER_H
#define TC_PLAYER_DBUS_MANAGER_H

void InitializeTMPlayerDBusManager(void);
void ReleaseTMPlayerDBusManager(void);
void SendDBusActiveApplication(int app, bool active);

int SendDBusSetAppID(int appID);
void SendDBusChangeContentType(void);
void SendDBusChangeNextDevice(bool play);
void SendDBusChangeDevice(unsigned char device, bool play);
void SendDBusSetBrowsingMode(unsigned int device, unsigned char content, bool meta);
void SendDBusChangeRepeatMode(void);
void SendDBusChangeShuffleMode(void);
void SendDBusPlayStart(void);
void SendDBusPlayStop(void);
void SendDBusPlayResume(void);
void SendDBusPlayPause(void);
void SendDBusTrackUp(void);
void SendDBusTrackDown(void);
void SendDBusTrackMove(int number);
void SendDBusTrackSeek(unsigned char hour, unsigned char min, unsigned char sec);
void SendDBusTrackForward(void);
void SendDBusTrackRewind(void);
void SendDBusTrackFFREWEnd(void);
void SendDBusUpdateVideoDisplayInfo(int x, int y, int width, int height);
void SendDBusUpdateVideoExtDisplayInfo(int x, int y, int width, int height);
void SendDBusRequestFileList(unsigned char device, unsigned char content, int number, unsigned int count);
void SendDBusMetaBrowsingStart(unsigned char device, unsigned int count);
void SendDBusMetaBrowsingRequest(unsigned char device, int index);
void SendDBusMetaBrowsingSelect(unsigned char device, int index);
void SendDBusMetaBrowsingHome(unsigned char device, unsigned int count);
void SendDBusMetaBrowsingUndo(unsigned char device);
void SendDBusMetaBrowsingEnd(unsigned char device, int index);
void SendDBusRequestDisplay(void);
void SendDBusGoLauncherHome(void);
void SendDBusGoLauncherBack(void);
void SendDBusGoAVOnOff(void);
void SendDBusNotifyDisplayDone(bool on);

#endif // TC_PLAYER_DBUS_MANAGER_H
