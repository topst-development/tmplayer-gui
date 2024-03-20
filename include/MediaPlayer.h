/****************************************************************************************
 *   FileName    : MediaPlayer.h
 *   Description : MediaPlayer.h
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

#ifndef MEDIA_PLAYER_H
#define MEDIA_PLAYER_H

#include <sys/types.h>
#include <QObject>
#include <QQuickWindow>
#include <QAbstractListModel>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QTimer>
#include "MediaPlayerType.h"
#include "DBusMsgDef.h"

class AlbumArt;
class PopupList;

class MediaPlayer : public QQuickWindow
{
    Q_OBJECT
	Q_PROPERTY(int device READ device NOTIFY changedDevice)
	Q_PROPERTY(int content READ content NOTIFY changedContent)
	Q_PROPERTY(QString playTimeString READ playTimeString CONSTANT)
	Q_PROPERTY(QString totalTimeString READ totalTimeString CONSTANT)
	Q_PROPERTY(bool noAlbumArt MEMBER _noAlbumArt NOTIFY noAlbumArtChanged)
	Q_PROPERTY(int repeat READ repeatStatus NOTIFY repeatChanged)
	Q_PROPERTY(bool shuffle READ shuffleStatus NOTIFY shuffleChanged)
	Q_PROPERTY(bool playStatus READ playStatus NOTIFY setPlayStatus)
	Q_PROPERTY(QString fileNameString READ fileNameString CONSTANT)
	Q_PROPERTY(bool fileListEnable READ fileListEnable NOTIFY fileDBCompleted)
	Q_PROPERTY(int fileNumber MEMBER _fileNumber NOTIFY changedFileNumber)
	Q_PROPERTY(int totalNumber MEMBER _totalNumber NOTIFY changedTotalNumber)
	Q_PROPERTY(int totalMetaNumber MEMBER _totalMetaNumber NOTIFY changedTotalMetaNumber)
	Q_PROPERTY(QString metaMenu MEMBER _metaMenu NOTIFY changedMetaMenu)
	Q_PROPERTY(int category READ category NOTIFY changedCategory)
	Q_PROPERTY(QAbstractListModel *list READ list NOTIFY changedList)

	public:
		MediaPlayer(QQuickWindow *parent = 0);
		~MediaPlayer();
		bool Initialize(void);

		int device() const;
		int content() const;
		QString playTimeString() const;
		QString totalTimeString() const;
		int repeatStatus() const;
		bool shuffleStatus() const;
		bool playStatus() const;
		QString fileNameString() const;
		bool fileListEnable() const;
		int category() const;
		QAbstractListModel *list() const;

		Q_INVOKABLE void goBack(void);
		Q_INVOKABLE void OnChangeDeviceSource(void);
		Q_INVOKABLE void OnChangeContent(void);
		Q_INVOKABLE void OnClearList(void);
		Q_INVOKABLE void OnRequestFileList(int requestNumber, unsigned int cnt);
		Q_INVOKABLE void OnStartMetaList(unsigned int cnt);
		Q_INVOKABLE void OnRequestMetaList(int requestNumber);
		Q_INVOKABLE void OnSelectMetaList(int selectedNumber);
		Q_INVOKABLE void OnHomeMetaList(int cnt);
		Q_INVOKABLE void OnUndoMetaList(void);
		Q_INVOKABLE void OnEndMetaList(int requestNumber);
		Q_INVOKABLE void OnListTrackNumberChanged(int number);
		Q_INVOKABLE void OnClickedRepeat();
		Q_INVOKABLE void OnClickedShuffle();
		Q_INVOKABLE void OnClickedPlayResume();
		Q_INVOKABLE void OnClickedPlayPause();
		Q_INVOKABLE void OnClickedTrackUp();
		Q_INVOKABLE void OnClickedTrackDown();
		Q_INVOKABLE void OnSliderValueChanged(int value);

	protected:
		void showEvent(QShowEvent *event);
		void hideEvent(QHideEvent *event);
		void moveEvent(QMoveEvent *event);
		void resizeEvent(QResizeEvent *event);

	private:
		void ShowPlayer(void);
		void HidePlayer(void);

		//common
		void ConnectSignalSlots();
		void ResetInfo(void);
		DeviceSource GetNextDevice();
		void ChangeContent(void);
		void ChangeDevice(DeviceSource device);
		//audio
		void SetID3Info(MetaCategory category, QString info);
		void SetAlbumArt(const unsigned char *buf, unsigned int length);
		void SetNoAlbumArt(void);
		//video
		void UpdateDisplayInfo(unsigned int x, unsigned int y, unsigned int width, unsigned int height);


	private slots:
		void OnProcessInitInformation(void);
		void OnProcessFailInit(void);
		void OnProcessPlayStateChanged(int status);
		void OnProcessPlayFailed(int code);
		void OnProcessResetInformation(void);
		void OnProcessFileNumberChange(int number);
		void OnProcessFileNumberUpdate(int device, int content,int number, int totalNumber);
		void OnProcessTotalNumberChange(int device, int content, int totalNumber);
		void OnProcessFileDBComplete(int device);
		void OnProcessFileNameChange(QString name);
		void OnProcessFolderNameChange(QString name);
		void OnProcessPlayTimeChange(unsigned char hour, unsigned char min, unsigned char sec);
		void OnProcessTotalTimeChange(unsigned char hour, unsigned char min, unsigned char sec);
		void OnProcessRepeatModeChange(unsigned int mode);
		void OnProcessShuffleModeChange(unsigned int mode);
		void OnProcessTrackSeekComplete(void);
		void OnProcessFileList(int device, int content, int number, QString name);
		void OnProcessFileListChanged(int device, int content, int number, int totalNumber);
		void OnProcessFileFolder(void);
		void OnProcessID3Info(int category, QString string);
		void OnProcessAlbumArtKey(int key, unsigned int size);
		void OnProcessAlbumArtCompleted(int number, unsigned int length);
		void OnProcessAlbumArtShmData(const char* uri, unsigned int length);

		void OnProcessNoDevice(void);
		void OnProcessContentChanged(int content);
		void OnProcessDeviceChanged(int device, int content);
		void OnProcessDeviceConnected(int device);
		void OnProcessDeviceDisconnected(int device);
		void OnProcessDeviceMounted(int device);
		void OnProcessDeviceMountStop(int device);
		void OnProcessNoSong(int device);

		void OnProcessDisplayOn();
		void OnProcessDisplayOff(void);

		void OnProcessNandDeleteStart(void);
		void OnProcessNandDeleteError(int number);
		void OnProcessNandDeleteOk(int number);
		void OnProcessNandDeleteAll(void);

		void OnMetaCountChanged(int device, int count);
		void OnMetaMenuChanged(int device, int category, QString menu);
		void OnMetaAddInfo(int device, int index, QString name, unsigned int type);
		void OnMetaCreateTrackList(int device, int count);

		void OnKeyboardClicked(int key);

	signals:
		void ChangeContentType();
		void ChangeNextMedia(bool play);
		void ChangeMedia(unsigned char device, bool play);
		void ReceiveAlbumArtComplete();
		void MediaPlayStart();
		void MediaPlayPause();
		void MediaPlayStop();
		void MediaResumePlay();
		void MediaTrackMove(int number);
		void SetBrowsingMode(unsigned int device, unsigned char, bool meta);
		void NotifyDisplayDone(bool on);

		void RequestFileList(int device, unsigned char content, int number, unsigned int count);
		void MetaBrowsingStart(unsigned int device, unsigned int count);
		void MetaBrowsingRequest(unsigned int device, int index);
		void MetaBrowsingSelect(unsigned int device, int index);
		void MetaBrowsingHome(unsigned int device, unsigned int count);
		void MetaBrowsingUndo(unsigned int device);
		void MetaBrowsingEnd(unsigned int device, int index);

		void RequestDisplay();
		void GoLauncherHome();
		void GoLauncherBack();
		void GoAVOnOff();

		void ShowLauncher(int app);
		void HideLauncher(int app);
		void ActiveApplication(int app, bool active);

		void backPressed();
		void upPressed();
		void downPressed();
		void prevPressed();
		void nextPressed();
		void okPressed();
		void repeatPressed();
		void shufflePressed();
		void changedDevice();
		void changedContent();
		void repeatChanged();
		void shuffleChanged();
		bool fileDBCompleted();
		void artistChanged(QString artistText);
		void albumChanged(QString albumText);
		void genreChanged(QString genreText);
		void fileIndexChanged(QString index);
		void noAlbumArtChanged();

		void setAudioPlayTime(QString time, int value);
		void setAudioTotalTime(QString time, int maxValue);
		void setVideoPlayTime(QString time, int value);
		void setVideoTotalTime(QString time, int maxValue);
		void setAudioFilename(QString text);
		void setVideoFilename(QString text);
		void setPlayStatus(bool play);

		void changedFileNumber();
		void changedTotalNumber();
		void changedTotalMetaNumber();
		void changedMetaMenu();
		void changedCategory();

//VideoPlayer
		void UpdateOverlayInfo(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
		void VideoPlayStart(void);
		void VideoPlayStop(void);
		void VideoPlayResume(void);
		void VideoPlayPause(void);
		void VideoPlayNext(void);
		void VideoPlayPrev(void);
		void VideoPlaySeek(unsigned char hour, unsigned char min, unsigned char sec);
		void VideoPlayForward(void);
		void VideoPlayRewind(void);
		void VideoPlayTurboForward(void);
		void VideoPlayTurboRewind(void);
		void VideoPlayFFREWEnd(void);
		void ClickedVideoList(void);
		void ClickedChangeContent(void);
//AudioPlayer
		void ChangeRepeatMode(void);
		void ChangeShuffleMode(void);
		void PlayStart(void);
		void PlayStop(void);
		void PlayResume(void);
		void PlayPause(void);
		void TrackUp(void);
		void TrackDown(void);
		void TrackSeek(unsigned char hour, unsigned char min, unsigned char sec);
		void TrackForward(void);
		void TrackRewind(void);
		void TrackTurboForward(void);
		void TrackTurboRewind(void);
		void TrackFFREWEnd(void);
		void changedList();

	private:
		DeviceSource 		_device;
		int					_fileNumber;
		int					_totalNumber;
		QString				_totalTime;
		QString				_playTime;
		bool 				_active;
		QImage              _albumArt;
		QImage              _albumArtBg;
		bool				_noAlbumArt;
		AlbumArt			*_albumArtPainter;
		AlbumArt			*_albumArtBgPainter;
		int					_totalMetaNumber;
		MetaType			_category;
		QString				_metaMenu;
		AVPlayStatus		_playStatus;
		bool				_userChangeMode;
		bool				_connectKeyEvent;
		RepeatMode			_repeatMode;
		ShuffleMode 		_shuffleMode;
		QString 			_fileName;

		MultiMediaContentType _contentType;
		bool _connectedDevices[TotalDeviceSources];
		key_t _albumArtKey;
		unsigned int _albumArtSize;
		int _albumArtID;
		PopupList *_list;
		bool _fileListCompleted[TotalDeviceSources];
};

class AlbumArt : public QQuickPaintedItem
{
	Q_OBJECT
		Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY changedImage)

	public:
		AlbumArt(QQuickPaintedItem *parent = 0);
		~AlbumArt();
		void paint(QPainter *painter);
		void SetAlbumArtImage(const QImage &albumArtImage);

		QImage image() const;

		Q_INVOKABLE void setImage(const QImage &image);

signals:
		void changedImage();

	private:
		QImage _image;
};

class PopupList : public QAbstractListModel
{
    Q_OBJECT

	enum PopupListRoles {
		indexNumber = Qt::UserRole + 1,
		FileName,
		Content
	};

	public:
		explicit PopupList(QAbstractListModel *parent = 0);
		~PopupList();
		void AddList(int index, QString name, MultiMediaContentType contentType);
		void ClearList(void);

		int rowCount(const QModelIndex &parent = QModelIndex()) const;
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	protected:
		QHash<int, QByteArray> roleNames() const;

	public:
		typedef struct _ListInfo {
			int _indexNumber;
			QString _fileName;
			MultiMediaContentType _contentType;
		} ListInfo;

	private:
		QList<ListInfo> _playList;
};
#endif // MEDIA_PLAYER_H
