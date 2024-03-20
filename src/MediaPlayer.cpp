/****************************************************************************************
 *   FileName    : MediaPlayer.cpp
 *   Description : MediaPlayer.cpp
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

#include <pwd.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QDebug>
#include <QImage>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include "DBusMsgDef.h"
#include "TCInput.h"
#include "TMPlayerGUIIFManager.h"
#include "TMPlayerGUIDBusManager.h"
#include "TCDBDefine.h"
#include "MediaPlayer.h"
#include "MediaPlayerType.h"

extern int g_debug;
extern int g_appID;
extern bool g_bt_ut;

#define DEBUG_MEDIA_PLAYER_PRINTF(format, arg...) \
	if (g_debug) \
	{ \
		fprintf(stderr, "[MEDIA PLAYER] %s: " format "", __FUNCTION__, ##arg); \
	}

#define ROWS_PER_PAGE				6

static char _myMusicPath[MAX_PATH_LENGTH];

extern bool CreateAlbumArtImage(QImage &destImage, QString data);
extern QTextCodec *g_codecUtf16;
extern QTextCodec *g_codecUtf8;

MediaPlayer::MediaPlayer(QQuickWindow *parent) :
	QQuickWindow(parent),
	_device(TotalDeviceSources),
	_fileNumber(0),
	_totalNumber(0),
	_totalTime("00:00:00"),
	_playTime("00:00:00"),
	_active(false),
	_noAlbumArt(true),
	_playStatus(PlayStatusNoStatement),
	_userChangeMode(false),
	_fileName("No Name"),
	_contentType(MultiMediaContentTypeAudio),
	_albumArtKey(-1),
	_albumArtSize(0),
	_albumArtID(-1)
{
	_list = new PopupList;
}

MediaPlayer::~MediaPlayer()
{
}

bool MediaPlayer::Initialize(void)
{
	bool init = false;
	QObject *temp;

	ConnectSignalSlots();

	struct passwd *pw = getpwuid(getuid());
	std::string myMusicDir;
	memset(_myMusicPath, 0x00, MAX_PATH_LENGTH);
	if (pw != NULL)
	{
		myMusicDir = pw->pw_dir;
	}
	else
	{
		fprintf(stderr, "get user home directory failed.\n set default path\n");
		myMusicDir = "/home/invite";
	}

	myMusicDir.append("/.telechips");
	mkdir(myMusicDir.c_str(), S_IRWXU|S_IRGRP|S_IXGRP);
	myMusicDir.append("/" MUSIC_DIR_NAME);
	strncpy(_myMusicPath, myMusicDir.c_str(), MAX_PATH_LENGTH - 1);
	mkdir(_myMusicPath, S_IRWXU|S_IRGRP|S_IXGRP);

	for (int i = 0; i < TotalDeviceSources; i++)
	{
		_connectedDevices[i] = false;
		_fileListCompleted[i] = false;
	}
	temp = this->findChild<QObject*>("albumArt");
	_albumArtPainter = qobject_cast<AlbumArt *>(temp);

	temp = this->findChild<QObject*>("albumArtBg");
	_albumArtBgPainter = qobject_cast<AlbumArt *>(temp);

	_albumArt.load(":/audio/images/audio_noalbum.png");
	_albumArtPainter->SetAlbumArtImage(_albumArt);
	_albumArtBg.load(":/images/background_01.png");
	_albumArtBgPainter->SetAlbumArtImage(_albumArtBg);

	emit noAlbumArtChanged();

	init = true;

	return init;
}

void MediaPlayer::UpdateDisplayInfo(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	emit UpdateOverlayInfo(x, y, width, height);
}

int MediaPlayer::device() const
{
	return (int)_device;
}

int MediaPlayer::content() const
{
	return (int)_contentType;
}

QString MediaPlayer::playTimeString() const
{
	return _playTime;
}

QString MediaPlayer::totalTimeString() const
{
	return _totalTime;
}

int MediaPlayer::repeatStatus() const
{
	return (int)_repeatMode;
}

bool MediaPlayer::shuffleStatus() const
{
	bool shuffle = false;
	if (_shuffleMode == ShuffleModeOn)
	{
		shuffle = true;
	}
	else
	{
		shuffle = false;
	}
	return shuffle;
}

bool MediaPlayer::playStatus() const
{
	bool play = false;

	if (_playStatus == PlayStatusPlaying)
	{
		play = true;
	}

	return play;
}

QString MediaPlayer::fileNameString() const
{
	return _fileName;
}

bool MediaPlayer::fileListEnable() const
{
	return _fileListCompleted[_device];
}

int MediaPlayer::category() const
{
	return (int)_category;
}

QAbstractListModel *MediaPlayer::list() const
{
	return (QAbstractListModel *)_list;
}

void MediaPlayer::goBack(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("BACK, SHOW LAUNCHER\n");

	emit GoLauncherBack();
}

void MediaPlayer::OnChangeDeviceSource(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE CHANGED\n");

	emit ChangeNextMedia(this->isVisible());
}

void MediaPlayer::OnClearList(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Clear List\n");

	_list->ClearList();
}

void MediaPlayer::OnRequestFileList(int requestNumber, unsigned int cnt)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Request file list: %d %d\n", _device, _contentType);

	emit RequestFileList(_device, _contentType, requestNumber, cnt);
}

void MediaPlayer::OnStartMetaList(unsigned int cnt)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Start meta list: %d\n", cnt);

	emit MetaBrowsingStart(_device, cnt);
}

void MediaPlayer::OnRequestMetaList(int requestNumber)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Request meta list: %d\n", requestNumber);

	emit MetaBrowsingRequest(_device, requestNumber);
}

void MediaPlayer::OnSelectMetaList(int selectedNumber)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Select meta list: %d\n", selectedNumber);

	emit MetaBrowsingSelect(_device, selectedNumber);
}

void MediaPlayer::OnHomeMetaList(int cnt)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Go home meta list: %d\n", cnt);

	emit MetaBrowsingHome(_device, cnt);
}

void MediaPlayer::OnUndoMetaList(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Undo meta list\n");

	emit MetaBrowsingUndo(_device);
}

void MediaPlayer::OnEndMetaList(int requestNumber)
{
	DEBUG_MEDIA_PLAYER_PRINTF("End meta list: %d\n", requestNumber);

	emit MetaBrowsingEnd(_device, requestNumber);
}

void MediaPlayer::OnListTrackNumberChanged(int number)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");

	emit MediaTrackMove(number);
}

void MediaPlayer::showEvent(QShowEvent *event)
{
	QQuickWindow::showEvent(event);

	emit HideLauncher(g_appID);
}

void MediaPlayer::hideEvent(QHideEvent * event)
{
	QQuickWindow::hideEvent(event);
}

void MediaPlayer::moveEvent(QMoveEvent *event)
{
	(void)event;
	UpdateDisplayInfo(this->x(), this->y(), this->width(), this->height());
}

void MediaPlayer::resizeEvent(QResizeEvent *event)
{
	(void)event;
	UpdateDisplayInfo(this->x(), this->y(), this->width(), this->height());
}

void MediaPlayer::ShowPlayer(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	this->show();
}

void MediaPlayer::HidePlayer(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	this->hide();
}

void MediaPlayer::OnProcessInitInformation(void)
{
	FILE *fp;

	fp = fopen("/var/run/TMPlayerGUI.pid", "w");
	if (fp != NULL)
	{
		fprintf(fp, "%lld\n", (long long int)getpid());
		fflush(fp);
		fclose(fp);
		fprintf(stderr, "%s : fopen(/var/run/TMPlayerGUI.pid) done\n",__PRETTY_FUNCTION__);
	}
	else
	{
		fprintf(stderr, "%s : fopen(/var/run/TMPlayerGUI.pid) failed, error(%d) - %s\n",__PRETTY_FUNCTION__, errno, strerror (errno));
	}

	for (int i = DeviceSourceMyMusic; i < TotalDeviceSources; i++)
	{
		if (i == DeviceSourceiAP2)
		{
			emit SetBrowsingMode(i, MultiMediaContentTypeAudio, true);
		}
		else
		{
			emit SetBrowsingMode(i, MultiMediaContentTypeAudio, false);
		}
	}
	UpdateDisplayInfo(this->x(), this->y(), this->width(), this->height());
}

void MediaPlayer::OnProcessFailInit(void)
{
	FILE *fp;

	fprintf(stderr, "%s : TMPlayer not responding\n",__PRETTY_FUNCTION__);

	fp = fopen("/var/run/TMPlayerGUI.pid", "w");
	if (fp != NULL)
	{
		fprintf(fp, "%lld\n", (long long int)getpid());
		fflush(fp);
		fclose(fp);
		DEBUG_MEDIA_PLAYER_PRINTF("Creating PID file is done\n");
	}
	else
	{
		fprintf(stderr, "%s : fopen(/var/run/TMPlayerGUI.pid) failed, error(%d) - %s\n",__PRETTY_FUNCTION__, errno, strerror (errno));
	}
}

void MediaPlayer::ChangeContent(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");

	if ( _contentType < TotalMultiMediaContentTypes)
	{
		emit changedContent();
	}
}

void MediaPlayer::ResetInfo(void)
{
	_playStatus = PlayStatusNoStatement;
	_totalTime = "00:00:00";
	_playTime = "00:00:00";
	_fileNumber = 0;
	_totalNumber = 0;
	_repeatMode = RepeatModeAll;
	_shuffleMode = ShuffleModeOff;
	_fileName = "";

	if (_contentType == MultiMediaContentTypeAudio)
	{
		emit artistChanged("");
		emit albumChanged("");
		emit genreChanged("");
		emit setAudioPlayTime(_playTime, 0);
		emit setAudioTotalTime(_totalTime, 0);
		emit setAudioFilename(_fileName);
		SetNoAlbumArt();
	}
	else if (_contentType == MultiMediaContentTypeVideo)
	{
		emit setVideoPlayTime(_playTime, 0);
		emit setVideoTotalTime(_totalTime, 0);
		emit setVideoFilename(_fileName);
	}
}

void MediaPlayer::ChangeDevice(DeviceSource device)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s)\n", device,
							  (device != TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN");

	if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
	{

		if( _device != device)
		{
			_device = device;		//Set current device

			ResetInfo();
			if(_fileListCompleted[device] == true)
			{
				emit fileDBCompleted();
			}
			emit changedDevice();
		}
	}
	else
	{
		fprintf(stderr, "%s: invalid device(%d)\n", __PRETTY_FUNCTION__, device);
	}
}

void MediaPlayer::ConnectSignalSlots()
{
	Qt::ConnectionType dbusConnectionType = Qt::QueuedConnection;


	//Audio Player
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(ID3Information(int, QString)),
			this, SLOT(OnProcessID3Info(int, QString)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(RepeatModeChange(unsigned int)),
			this, SLOT(OnProcessRepeatModeChange(unsigned int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(ShuffleModeChange(unsigned int)),
			this, SLOT(OnProcessShuffleModeChange(unsigned int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(AlbumArtKey(int, unsigned int)),
			this, SLOT(OnProcessAlbumArtKey(int, unsigned int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(AlbumArtCompleted(int, unsigned int)),
			this, SLOT(OnProcessAlbumArtCompleted(int, unsigned int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(AlbumArtShmData(const char*, unsigned int)),
			this, SLOT(OnProcessAlbumArtShmData(const char*, unsigned int)), dbusConnectionType);
	//Video Player
	connect(this, SIGNAL(UpdateOverlayInfo(uint,uint,uint,uint)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnUpdateOverlayInfo(uint,uint,uint,uint)), Qt::QueuedConnection);

	//Common
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(InitInformation()),
			this, SLOT(OnProcessInitInformation()), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FailInit()),
			this, SLOT(OnProcessFailInit()), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(PlayStateChanged(int)),
			this, SLOT(OnProcessPlayStateChanged(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(PlayFailed(int)),
			this, SLOT(OnProcessPlayFailed(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(PlayTimeChange(unsigned char, unsigned char, unsigned char)),
			this, SLOT(OnProcessPlayTimeChange(unsigned char, unsigned char, unsigned char)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(TotalTimeChange(unsigned char, unsigned char, unsigned char)),
			this, SLOT(OnProcessTotalTimeChange(unsigned char, unsigned char, unsigned char)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(SeekComplete()),
			this, SLOT(OnProcessTrackSeekComplete()), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(NoDevice()),
			this, SLOT(OnProcessNoDevice()), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(ContentChanged(int)),
			this, SLOT(OnProcessContentChanged(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DeviceChanged(int, int)),
			this, SLOT(OnProcessDeviceChanged(int, int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DeviceConnected(int)),
			this, SLOT(OnProcessDeviceConnected(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DeviceDisconnected(int)),
			this, SLOT(OnProcessDeviceDisconnected(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DeviceMounted(int)),
			this, SLOT(OnProcessDeviceMounted(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DeviceMountStop(int)),
			this, SLOT(OnProcessDeviceMountStop(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(NoSong(int)),
			this, SLOT(OnProcessNoSong(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(ResetInformation()),
			this, SLOT(OnProcessResetInformation()), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FileNumberChange(int)),
			this, SLOT(OnProcessFileNumberChange(int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FileNumberUpdate(int, int, int, int)),
			this, SLOT(OnProcessFileNumberUpdate(int, int, int, int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(TotalNumberChange(int, int, int)),
			this, SLOT(OnProcessTotalNumberChange(int, int, int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FileDBComplete(int)),
			this, SLOT(OnProcessFileDBComplete(int)), dbusConnectionType);	
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FileNameChange(QString)),
			this, SLOT(OnProcessFileNameChange(QString)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FolderNameChange(QString)),
			this, SLOT(OnProcessFolderNameChange(QString)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FileList(int, int, int, QString)),
			this, SLOT(OnProcessFileList(int, int, int, QString)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(FileListChanged(int, int, int, int)),
			this, SLOT(OnProcessFileListChanged(int, int, int, int)), dbusConnectionType);

	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DisplayOn()),
			this, SLOT(OnProcessDisplayOn()), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(DisplayOff()),
			this, SLOT(OnProcessDisplayOff()), dbusConnectionType);

	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(MetaCountChanged(int, int)),
			this, SLOT(OnMetaCountChanged(int, int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(MetaMenuChanged(int, int, QString)),
			this, SLOT(OnMetaMenuChanged(int, int, QString)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(MetaAddInfo(int, int, QString, unsigned int)),
			this, SLOT(OnMetaAddInfo(int, int, QString, unsigned int)), dbusConnectionType);
	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(MetaCreateTrackList(int, int)),
			this, SLOT(OnMetaCreateTrackList(int, int)), dbusConnectionType);

	connect(TM_PLAYER_GUI_IF_MANAGER, SIGNAL(KeyboardClicked(int)),
			this, SLOT(OnKeyboardClicked(int)), dbusConnectionType);

	connect(this, SIGNAL(NotifyDisplayDone(bool)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnNotifyDisplayDone(bool)), dbusConnectionType);

	connect(this, SIGNAL(ChangeContentType()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnChangeContentType()), dbusConnectionType);
	connect(this, SIGNAL(ChangeNextMedia(bool)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnChangeNextDevice(bool)), dbusConnectionType);
	connect(this, SIGNAL(ChangeMedia(unsigned char, bool)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnChangeDevice(unsigned char, bool)), dbusConnectionType);
	connect(this, SIGNAL(MediaPlayStart()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnPlayStart()), dbusConnectionType);
	connect(this, SIGNAL(MediaPlayStop()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnPlayStop()), dbusConnectionType);
	connect(this, SIGNAL(MediaResumePlay()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnPlayResume()), dbusConnectionType);
	connect(this, SIGNAL(MediaPlayPause()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnPlayPause()), dbusConnectionType);
	connect(this, SIGNAL(MediaTrackMove(int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnTrackMove(int)), dbusConnectionType);
	connect(this, SIGNAL(ChangeRepeatMode()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnChangeRepeatMode()), dbusConnectionType);
	connect(this, SIGNAL(ChangeShuffleMode()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnChangeShuffleMode()), dbusConnectionType);
	connect(this, SIGNAL(PlayResume()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnPlayResume()), dbusConnectionType);
	connect(this, SIGNAL(PlayPause()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnPlayPause()), dbusConnectionType);
	connect(this, SIGNAL(TrackUp()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnTrackUp()), dbusConnectionType);
	connect(this, SIGNAL(TrackDown()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnTrackDown()), dbusConnectionType);
	connect(this, SIGNAL(TrackSeek(unsigned char, unsigned char, unsigned char)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnTrackSeek(unsigned char, unsigned char, unsigned char)), dbusConnectionType);


	connect(this, SIGNAL(RequestFileList(int, unsigned char, int, unsigned int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnRequestFileList(int, unsigned char, int, unsigned int)), dbusConnectionType);
	connect(this, SIGNAL(MetaBrowsingStart(unsigned int, unsigned int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnMetaBrowsingStart(unsigned int, unsigned int)), dbusConnectionType);
	connect(this, SIGNAL(MetaBrowsingRequest(unsigned int, int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnMetaBrowsingRequest(unsigned int, int)), dbusConnectionType);
	connect(this, SIGNAL(MetaBrowsingSelect(unsigned int, int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnMetaBrowsingSelect(unsigned int, int)), dbusConnectionType);
	connect(this, SIGNAL(MetaBrowsingHome(unsigned int, unsigned int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnMetaBrowsingHome(unsigned int, unsigned int)), dbusConnectionType);
	connect(this, SIGNAL(MetaBrowsingUndo(unsigned int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnMetaBrowsingUndo(unsigned int)), dbusConnectionType);
	connect(this, SIGNAL(MetaBrowsingEnd(unsigned int, int)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnMetaBrowsingEnd(unsigned int, int)), dbusConnectionType);
	connect(this, SIGNAL(SetBrowsingMode(unsigned int, unsigned char, bool)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnSetBrowsingMode(unsigned int, unsigned char, bool)), dbusConnectionType);

	connect(this, SIGNAL(RequestDisplay()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnRequestDisplay()), dbusConnectionType);
	connect(this, SIGNAL(GoLauncherHome()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnGoLauncherHome()), dbusConnectionType);
	connect(this, SIGNAL(GoLauncherBack()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnGoLauncherBack()), dbusConnectionType);
	connect(this, SIGNAL(GoAVOnOff()),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnGoAVOnOff()), dbusConnectionType);

	// Launcher SIGNAL/SLOT
	connect(this, SIGNAL(ActiveApplication(int, bool)),
			TM_PLAYER_GUI_IF_MANAGER, SLOT(OnActiveApplication(int,bool)), dbusConnectionType);
}

DeviceSource MediaPlayer::GetNextDevice()
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	DeviceSource device = TotalDeviceSources;

	for (int i = 0; (i < TotalDeviceSources) && (device == TotalDeviceSources); i++)
	{
		if (_connectedDevices[i])
		{
			device = (DeviceSource)i;
		}
	}

	return device;
}

void MediaPlayer::OnProcessPlayStateChanged(int status)
{
	DEBUG_MEDIA_PLAYER_PRINTF("STATUS(%d)\n",
							  status);

	if (status == PlayStatusPlaying)
	{
		if (_playStatus != PlayStatusFF &&
			_playStatus != PlayStatusRew &&
			_playStatus != PlayStatusTurboFF &&
			_playStatus != PlayStatusTurboRew)
		{
			_playStatus = PlayStatusPlaying;
			emit setPlayStatus(true);
		}
	}
	else if (status == PlayStatusPause)
	{
		_playStatus = PlayStatusPause;
		emit setPlayStatus(false);
	}
	else if (status == PlayStatusStop)
	{
		_playStatus = PlayStatusStop;
		emit setPlayStatus(false);
	}
	else if (status == PlayStatusFF)
	{
		_playStatus = (AVPlayStatus)status;
	}
	else if (status == PlayStatusRew)
	{
		_playStatus = (AVPlayStatus)status;
	}
	else if (status == PlayStatusEndFFRew)
	{
		_playStatus = PlayStatusPlaying;
		emit setPlayStatus(true);
	}
}

void MediaPlayer::OnProcessPlayFailed(int code)
{
	fprintf(stderr, "ERROR CODE(%d)\n", code);
}

void MediaPlayer::OnProcessResetInformation(void)
{
	ResetInfo();
}

void MediaPlayer::OnProcessFileNumberChange(int number)
{
	DEBUG_MEDIA_PLAYER_PRINTF("NUMBER(%u)\n", number);

	if(_userChangeMode)
	{
		_fileNumber = number;
		_userChangeMode = false;
	}
	else if(_fileNumber != number)
	{
		int totalNumber = _totalNumber;
		ResetInfo();
		_fileNumber = number;
		_totalNumber = totalNumber;
	}

	if (_contentType == MultiMediaContentTypeAudio)
	{
		emit fileIndexChanged(QString("%1 / %2").arg(number).arg(_totalNumber));
	}

	emit changedFileNumber();
}

void MediaPlayer::OnProcessFileNumberUpdate(int device, int content,int number, int totalNumber)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s), CONTENT(%d), NUMBER(%u), TOTAL(%u)\n",
							  device,
							  (device != TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN",
							  content, number, totalNumber);

	if(device == _device)
	{
		/* Set file Index */
		if ((_contentType == MultiMediaContentTypeAudio)&&(_contentType == content))
		{
			emit fileIndexChanged(QString("%1 / %2").arg(number).arg(totalNumber));
		}
		else if ((_contentType == MultiMediaContentTypeVideo)&&(_contentType == content))
		{
			ResetInfo();
		}

		/* Set total&file Number */
		_fileNumber = number;
		_totalNumber = totalNumber;

		emit changedFileNumber();
		emit changedTotalNumber();
	}
}

void MediaPlayer::OnProcessTotalNumberChange(int device, int content, int totalNumber)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s), CONTENT(%d), NUMBER(%u)\n",
							  device,
							  (device != TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN",
							  content, totalNumber);

	if (content == MultiMediaContentTypeAudio)
	{
		emit fileIndexChanged(QString("%1 / %2").arg(_fileNumber).arg(totalNumber));
	}

	_totalNumber = totalNumber;
	emit changedTotalNumber();
}

void MediaPlayer::OnProcessFileDBComplete(int device)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d])\n", device);

	_fileListCompleted[device] = true;

	if(device == _device)
	{
		emit fileDBCompleted();
	}
}

void MediaPlayer::OnProcessFileNameChange(QString name)
{
	DEBUG_MEDIA_PLAYER_PRINTF("NAME(%s)\n", name.toLatin1().data());

	if (_contentType == MultiMediaContentTypeAudio)
	{
		emit setAudioFilename(name);
	}
	else if (_contentType == MultiMediaContentTypeVideo)
	{
		emit setVideoFilename(name);
	}
}

void MediaPlayer::OnProcessFolderNameChange(QString name)
{
	DEBUG_MEDIA_PLAYER_PRINTF("NAME(%s)\n", name.toLatin1().data());
}

void MediaPlayer::OnProcessPlayTimeChange(unsigned char hour, unsigned char min, unsigned char sec)
{
	QString time = QString("%1:%2:%3")
				   .arg(hour, 2, 10, QChar('0'))
				   .arg(min, 2, 10, QChar('0'))
				   .arg(sec, 2, 10, QChar('0'));
	int playValue = hour * 3600 + min * 60 + sec;

	_playTime = time;

	if (_contentType == MultiMediaContentTypeAudio)
	{
		emit setAudioPlayTime(_playTime, playValue);
	}
	else if (_contentType == MultiMediaContentTypeVideo)
	{
		emit setVideoPlayTime(_playTime, playValue);
	}
}

void MediaPlayer::OnProcessTotalTimeChange(unsigned char hour, unsigned char min, unsigned char sec)
{
	DEBUG_MEDIA_PLAYER_PRINTF("HOUR(%u), MIN(%u), SEC(%u)\n", hour, min, sec);
	QString time = QString("%1:%2:%3")
				   .arg(hour, 2, 10, QChar('0'))
				   .arg(min, 2, 10, QChar('0'))
				   .arg(sec, 2, 10, QChar('0'));
	int maxValue = hour * 3600 + min * 60 + sec;

	_totalTime = time;

	if (_contentType == MultiMediaContentTypeAudio)
	{
		setAudioTotalTime(_totalTime, maxValue);
	}
	else if (_contentType == MultiMediaContentTypeVideo)
	{
		setVideoTotalTime(_totalTime, maxValue);
	}
}

void MediaPlayer::OnProcessRepeatModeChange(unsigned int mode)
{
	_repeatMode = (RepeatMode)mode;

	DEBUG_MEDIA_PLAYER_PRINTF("MODE(%d)\n", mode);

	if (_repeatMode < TotalRepeatModes)
	{
		if (_contentType == MultiMediaContentTypeAudio)
		{
			emit repeatChanged();
		}
	}
	else
	{
		fprintf(stderr, "%s: undefined play mode value(%u)\n", __PRETTY_FUNCTION__, _repeatMode);
	}
}

void MediaPlayer::OnProcessShuffleModeChange(unsigned int mode)
{
	_shuffleMode = (ShuffleMode)mode;

	DEBUG_MEDIA_PLAYER_PRINTF("MODE(%d)\n", mode);

	if (_shuffleMode < TotalShuffleModes)
	{
		if (_contentType == MultiMediaContentTypeAudio)
		{
			emit shuffleChanged();
		}
	}
	else
	{
		fprintf(stderr, "%s: undefined play mode value(%u)\n", __PRETTY_FUNCTION__, _shuffleMode);
	}
}

void MediaPlayer::OnProcessTrackSeekComplete(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
}

void MediaPlayer::OnProcessFileList(int device, int content, int number, QString name)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE(%d), CONTENT(%d), NUMBER(%d), NAME(%s)\n",
							  device, content, number, name.toLatin1().data());

	if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
	{
		if (number <= _totalNumber)
		{
			_list->AddList(number, name, (MultiMediaContentType)content);

			emit changedList();
		}
	}
}

void MediaPlayer::OnProcessFileListChanged(int device, int content, int number, int totalNumber)
{
	(void)device;
	(void)content;

	DEBUG_MEDIA_PLAYER_PRINTF("NUMBER (%d) TOTAL NUMBER (%d)\n", number, totalNumber);

	_fileNumber = number;
	_totalNumber = totalNumber;

	emit changedFileNumber();
	emit changedTotalNumber();
}

void MediaPlayer::OnProcessFileFolder(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
}

void MediaPlayer::OnProcessID3Info(int category, QString string)
{
	DEBUG_MEDIA_PLAYER_PRINTF("CATEGORY(%d), Information(%s)\n", category, string.toLatin1().data());

	if (_contentType == MultiMediaContentTypeAudio)
	{
		SetID3Info((MetaCategory)category, string);
	}
	else if (_contentType == MultiMediaContentTypeVideo)
	{
	}
}

void MediaPlayer::OnProcessAlbumArtKey(int key, unsigned int size)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	_albumArtKey = key;
	_albumArtSize = size;
	_albumArtID = shmget(_albumArtKey, _albumArtSize, 0);
}

void MediaPlayer::OnProcessAlbumArtCompleted(int number, unsigned int length)
{
	(void)number;

	DEBUG_MEDIA_PLAYER_PRINTF("\n");

	if (_contentType == MultiMediaContentTypeAudio)
	{
		if (_albumArtKey != -1 && _albumArtSize > 0)
		{
			if (_albumArtID != -1)
			{
				struct shmid_ds shmStat;

				if (shmctl(_albumArtID, IPC_STAT, &shmStat) != -1)
				{
					if (shmStat.shm_segsz >= length)
					{
						void *buf;

						buf = shmat(_albumArtID, (void *)0, SHM_RDONLY);
						if (buf != (void *)-1)
						{
							SetAlbumArt((const unsigned char *)buf, length);

							if (shmdt(buf) == -1)
							{
								fprintf(stderr, "%s:", __PRETTY_FUNCTION__);
								perror("shmdt failed");
							}
						}
						else
						{
							fprintf(stderr, "%s:", __PRETTY_FUNCTION__);
							perror("shmat failed: ");
						}
					}
					else
					{
						fprintf(stderr, "%s: not enough memory. album art length(%u), shm segment size(%lu)\n", __PRETTY_FUNCTION__,
								length, shmStat.shm_segsz);
					}
				}
				else
				{
					fprintf(stderr, "%s:", __PRETTY_FUNCTION__);
					perror("shmctl failed");
					exit(1);
				}
			}
			else
			{
				fprintf(stderr, "%s:", __PRETTY_FUNCTION__);
				perror("shmget failed: ");
			}
		}
		else
		{
			fprintf(stderr, "%s: not initialize shared memory information. key(%d), size(%u)\n", __PRETTY_FUNCTION__,
					_albumArtKey, _albumArtSize);
		}
	}
}

void MediaPlayer::OnProcessAlbumArtShmData(const char* uri, unsigned int length)
{
    DEBUG_MEDIA_PLAYER_PRINTF("\n");
	char *albumArtBuf;
	FILE *fp;

	if (_contentType == MultiMediaContentTypeAudio)
	{
		if (length > 0)
		{
			if((fp = fopen(uri, "r")) == NULL)
			{
				fprintf(stderr, "%s: Error fopen AlbumArt file (%s)\n", __FUNCTION__, uri);
			}
			else
			{
				albumArtBuf = (char *)malloc(length);
				if(albumArtBuf == NULL)
				{
					fprintf(stderr, "%s: Error malloc AlbumArt\n", __FUNCTION__);
				}
				else
				{
					char *bufPtr= albumArtBuf;
					unsigned int bytesToRead = length;
					unsigned int bufSize = 0;
					bool failed = false;

					while (!failed && bytesToRead)
					{
						bufSize = fread(bufPtr, 1, bytesToRead, fp);
						if (bufSize > 0)
						{
							bufPtr += bufSize;
							bytesToRead -= bufSize;
						}
						else
						{
							failed = true;
						}
					}

					bufSize = length - bytesToRead;
					SetAlbumArt((const unsigned char *)albumArtBuf, bufSize);
					free(albumArtBuf);

					if (bufSize != length)
					{
						fprintf(stderr, "%s: file read failed. length(%d), bytes to read(%d)\n", __PRETTY_FUNCTION__, length, bufSize);
					}
				}

				if (fclose(fp) < 0)
				{
					fprintf(stderr, "%s: fclose failed\n", __PRETTY_FUNCTION__);
				}
			}
		}
		else
		{
			SetNoAlbumArt();
			fprintf(stderr, "%s: No Album Art\n", __FUNCTION__);
		}
	}
}

void MediaPlayer::OnProcessNoDevice(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");

	if (_active)
	{
		_active = false;
		emit ActiveApplication(g_appID, _active);
	}
}

void MediaPlayer::OnProcessContentChanged(int content)
{
	DEBUG_MEDIA_PLAYER_PRINTF("CONTENT(%d)\n", content);

	if (_contentType < TotalMultiMediaContentTypes)
	{
		_contentType = (MultiMediaContentType)content;
		ChangeContent();

		if (this->isVisible())
		{
			ShowPlayer();
		}
	}
	else
	{
		DEBUG_MEDIA_PLAYER_PRINTF("CONTENT CHANGE FAILED\n");
	}
}

void MediaPlayer::OnProcessDeviceChanged(int device, int content)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s) CONTENT(%d)\n",
							  device,
							  (device >= 0 && device < TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN",
							  content);

	_contentType = (MultiMediaContentType)content;

	ChangeDevice((DeviceSource)device);
	ChangeContent();
	if (this->isVisible())
	{
		ShowPlayer();
	}
}

void MediaPlayer::OnProcessDeviceConnected(int device)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s)\n",
							  device,
							  (device >= 0 && device < TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN");

	DeviceSource dev;

	dev = (DeviceSource)device;

	if (dev >= DeviceSourceMyMusic && dev < TotalDeviceSources)
	{
		_connectedDevices[dev] = true;
		_fileListCompleted[dev] = false;
	}
	else
	{
		fprintf(stderr, "%s: invalid device(%d)\n", __PRETTY_FUNCTION__, dev);
	}
}

void MediaPlayer::OnProcessDeviceDisconnected(int device)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s)\n", device,
							  (device >= 0 && device < TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN");

	DeviceSource dev;

	dev = (DeviceSource)device;
	if (dev >= DeviceSourceMyMusic && dev < TotalDeviceSources)
	{
		_connectedDevices[dev] = false;
		_fileListCompleted[dev] = false;

		if (dev == _device)	// if disconnect device == playing device,
		{
			_device = TotalDeviceSources;
			ResetInfo();

			emit changedDevice();
		}
	}
	else
	{
		fprintf(stderr, "%s: invalid device(%d)\n", __PRETTY_FUNCTION__, dev);
	}
}

void MediaPlayer::OnProcessDeviceMounted(int device)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s)\n", device,
							  (device >= 0 && device < TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN");

	if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
	{
		if (!_active)
		{
			_active = true;
			emit ActiveApplication(g_appID, _active);
		}
	}
	else
	{
		fprintf(stderr, "%s: invalid device(%d)\n", __PRETTY_FUNCTION__, device);
	}
}

void MediaPlayer::OnProcessDeviceMountStop(int device)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE([%d] %s)\n",
							  device,
							  (device >= 0 && device < TotalDeviceSources) ? g_deviceSourceNames[device] : "UNKNOWN");
	DeviceSource dev;

	dev = (DeviceSource)device;
	if (dev >= DeviceSourceMyMusic && dev < TotalDeviceSources)
	{
		if (dev == _device)
		{
			_device = TotalDeviceSources;

			emit changedDevice();
		}
	}
	else
	{
		fprintf(stderr, "%s: invalid device(%d)\n", __PRETTY_FUNCTION__, dev);
	}
}

void MediaPlayer::OnProcessNoSong(int device)
{
	DeviceSource dev;

	dev = (DeviceSource)device;
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	if (dev == _device)
	{
		_device = TotalDeviceSources;

		emit changedDevice();
	}
}

void MediaPlayer::OnProcessDisplayOn()
{
	ShowPlayer();
	emit NotifyDisplayDone(true);
}

void MediaPlayer::OnProcessDisplayOff(void)
{
	HidePlayer();
	DEBUG_MEDIA_PLAYER_PRINTF("Display Off\n");
	emit NotifyDisplayDone(false);
}

void MediaPlayer::OnProcessNandDeleteStart(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
}

void MediaPlayer::OnProcessNandDeleteError(int number)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	(void)number;
}

void MediaPlayer::OnProcessNandDeleteOk(int number)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
	if (_contentType == MultiMediaContentTypeAudio)
	{
	}
	(void)number;
}

void MediaPlayer::OnProcessNandDeleteAll(void)
{
	DEBUG_MEDIA_PLAYER_PRINTF("\n");
}

void MediaPlayer::OnMetaCountChanged(int device, int count)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE(%d), COUNT(%d)\n",
							  device, count);
	if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
	{
		_totalMetaNumber = count;

		emit changedTotalMetaNumber();
	}
}

void MediaPlayer::OnMetaMenuChanged(int device, int category, QString menu)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE(%d), CATEGORY(%d), MENU(%s)\n",
							  device, category, menu.toLatin1().data());
	if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
	{
		_category = (MetaType)category;
		_metaMenu = menu;

		emit changedCategory();
		emit changedMetaMenu();
	}
}

void MediaPlayer::OnMetaAddInfo(int device, int index, QString name, unsigned int type)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE(%d), INDEX(%d), NAME(%s) TYPE(%d)\n",
							  device, index, name.toLatin1().data(), type);
	if (device >= DeviceSourceMyMusic && device < TotalDeviceSources)
	{
		_list->AddList(index, name, MultiMediaContentTypeAudio);

		emit changedList();
	}
}

void MediaPlayer::OnMetaCreateTrackList(int device, int count)
{
	DEBUG_MEDIA_PLAYER_PRINTF("DEVICE(%d), count(%d)\n", device, count);
	if (_device == device)
	{
		_totalNumber = count;

		if (this->isVisible() && _contentType == MultiMediaContentTypeAudio)
		{
			emit MediaPlayStart();
		}

		emit changedTotalNumber();
	}
}

void MediaPlayer::OnKeyboardClicked(int key)
{
	DEBUG_MEDIA_PLAYER_PRINTF("Clicked Key: %d\n", key);

	if (key == g_knobKeys[TCKeyPower])
	{
		DEBUG_MEDIA_PLAYER_PRINTF("Clicked Key: AV On/Off\n");

		emit GoAVOnOff();
	}

	if (this->isVisible())
	{
		if (key == g_knobKeys[TCKeyHome])
		{
			DEBUG_MEDIA_PLAYER_PRINTF("Go TO THE HOME. SHOW LAUNCHER\n");
			if (_contentType == MultiMediaContentTypeVideo)
			{
				emit MediaPlayStop();
			}
			emit GoLauncherHome();
		}
		else if (key == g_knobKeys[TCKeyMedia])
		{
			DEBUG_MEDIA_PLAYER_PRINTF("DEVICE CHANGED!\n");

			emit ChangeNextMedia(this->isVisible());
		}
		else if (key == g_knobKeys[TCKeyBack])
		{
			emit backPressed();
		}
		else if (key == g_knobKeys[TCKeyUp])
		{
			emit upPressed();
		}
		else if (key == g_knobKeys[TCKeyDown])
		{
			emit downPressed();
		}
		else if (key == g_knobKeys[TCKeyLeft] || key == g_knobKeys[TCKeyPrev])
		{
			emit prevPressed();
		}
		else if (key == g_knobKeys[TCKeyRight] || key == g_knobKeys[TCKeyNext])
		{
			emit nextPressed();
		}
		else if (key == g_knobKeys[TCKeyOk] || key == g_knobKeys[TCKeyHome])
		{
			emit okPressed();
		}
		else if (key == g_knobKeys[TCKey1])
		{
			emit repeatPressed();
		}
		else if (key == g_knobKeys[TCKey2])
		{
			emit shufflePressed();
		}
	}
	else if (key == g_knobKeys[TCKeyMedia])
	{
		if (_device != TotalDeviceSources)
		{
			emit RequestDisplay();
		}
	}
}

void MediaPlayer::OnChangeContent(void)
{
	emit ChangeContentType();
}

void MediaPlayer::OnClickedPlayResume()
{
	if (_playStatus == PlayStatusStop)
	{
		emit PlayStart();
	}
	else
	{
		emit PlayResume();
	}
}

void MediaPlayer::OnClickedPlayPause()
{
	emit PlayPause();
}

void MediaPlayer::OnClickedTrackUp()
{
	emit TrackUp();
}

void MediaPlayer::OnClickedTrackDown()
{
	emit TrackDown();
}

void MediaPlayer::OnSliderValueChanged(int value)
{
	unsigned int hour, min, sec;
	sec = value;
	min = sec / 60;
	sec %= 60;
	hour = min / 60;
	min %= 60;

	DEBUG_MEDIA_PLAYER_PRINTF("Seek Value Changed %d, min(%d), sec(%d)\n", value, min, sec);

	if(_playStatus != PlayStatusStop || _playStatus != PlayStatusNoStatement)
	{
		if (_playStatus == PlayStatusPause)
		{
			emit PlayResume();
		}

		emit TrackSeek((unsigned char)hour,(unsigned char)min, (unsigned char)sec);
	}
}

bool CreateAlbumArtImage(QImage &destImage, QString data)
{
	bool complete = false;
	QStringList parameters = data.split(";");
	QString type = parameters.at(0);

	if (type == "SHM_OPEN")
	{
		QString objPath;
		int length, width, height;
		QImage::Format bpp;

		objPath = parameters.at(1);
		length = QString(parameters.at(2)).toInt();
		width = QString(parameters.at(3)).toInt();
		height = QString(parameters.at(4)).toInt();
		bpp = (QImage::Format)QString(parameters.at(5)).toInt();

		int fd = shm_open(objPath.toLocal8Bit().data(), O_RDONLY, S_IRUSR | S_IRGRP);
		if (fd >= 0)
		{
			unsigned char *srcBuf = (unsigned char *)mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
			if (srcBuf != NULL)
			{
				if (bpp == QImage::Format_Invalid)
				{
					QImage image;
					if (image.loadFromData(srcBuf, length))
					{
						//destImage = image.scaled(ALBUM_ART_WIDTH, ALBUM_ART_HEIGHT);
						destImage = image;
						complete = true;
					}
					else
					{
						fprintf(stderr, "%s: loadFromData failed\n", __PRETTY_FUNCTION__);
					}
				}
				else
				{
					QImage image(width, height, bpp);
					unsigned char *destBuf = image.bits();

					if (destBuf != NULL)
					{
						if (length == image.byteCount())
						{
							memcpy(destBuf, srcBuf, length);
							//destImage = image.scaled(ALBUM_ART_WIDTH, ALBUM_ART_HEIGHT);
							destImage = image;
							complete = true;
						}
						else
						{
							fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);
							fprintf(stderr, "length check failed\n");
						}
					}
				}

				munmap(srcBuf, length);
			}
			else
			{
				fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);
				perror("mmap failed: ");
			}
			close(fd);
		}
		else
		{
			fprintf(stderr, "%s: ", __PRETTY_FUNCTION__);
			perror("shm_open failed: ");
		}
	}

	return complete;
}

AlbumArt::AlbumArt(QQuickPaintedItem *parent) :
	QQuickPaintedItem(parent)
{
}

AlbumArt::~AlbumArt()
{
}

void AlbumArt::paint(QPainter *painter)
{
	QImage scaled = _image.scaled(boundingRect().width(), boundingRect().height());
	QPointF center = boundingRect().center() - scaled.rect().center();

	if(center.x() < 0)
	{
		center.setX(0);
	}
	if(center.y() < 0)
	{
		center.setY(0);
	}

	painter->drawImage(center, scaled);
	DEBUG_MEDIA_PLAYER_PRINTF("paint image\n");
}

void AlbumArt::SetAlbumArtImage(const QImage &albumArtImage)
{
	DEBUG_MEDIA_PLAYER_PRINTF("update image\n");
	_image = albumArtImage;
	update();
}

QImage AlbumArt::image() const
{
	DEBUG_MEDIA_PLAYER_PRINTF("return image\n");
	return this->_image;
}

void AlbumArt::setImage(const QImage &image)
{
	DEBUG_MEDIA_PLAYER_PRINTF("set new image\n");
	this->_image = image;
	emit changedImage();
	update();
}

PopupList::PopupList(QAbstractListModel *parent) :
	QAbstractListModel(parent)
{
}

PopupList::~PopupList()
{
}

void PopupList::AddList(int index, QString name, MultiMediaContentType contentType)
{
	ListInfo listInfo;

	listInfo._indexNumber = index;
	listInfo._fileName = name;
	listInfo._contentType = contentType;

	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	_playList.push_back(listInfo);
	endInsertRows();
}

void PopupList::ClearList(void)
{
	beginResetModel();
	_playList.clear();
	endResetModel();
}

int PopupList::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return _playList.count();
}

QVariant PopupList::data(const QModelIndex & index, int role) const
{
	QVariant var;

	if (index.row() > 0 || index.row() < _playList.count())
	{
		ListInfo listInfo = _playList[index.row()];

		if (role == indexNumber)
		{
			var = listInfo._indexNumber;
		}
		else if (role == FileName)
		{
			var = listInfo._fileName;
		}
		else if (role == Content)
		{
			var = (int)listInfo._contentType;
		}
	}

	return var;
}

QHash<int, QByteArray> PopupList::roleNames() const
{
	QHash<int, QByteArray> roles;

	roles[indexNumber] = "indexNumber";
	roles[FileName] = "fileName";
	roles[Content] = "content";

	return roles;
}
