/****************************************************************************************
 *   FileName    : AudioPlayer.cpp
 *   Description : AudioPlayer.cpp
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

#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <QTextCodec>
#include <pwd.h>
#include <QDebug>
#include <QImage>
#include <QKeyEvent>
#include "TCInput.h"
#include "TMPlayerGUIIFManager.h"
#include "MediaPlayer.h"
#include "TCDBDefine.h"

#define ALBUM_ART_WIDTH		334
#define ALBUM_ART_HEIGHT	334

extern int g_debug;

#define DEBUG_AUDIO_PLAYER_PRINTF(format, arg...) \
	if (g_debug) \
	{ \
		fprintf(stderr, "[AUDIO PLAYER] %s: " format "", __FUNCTION__, ##arg); \
	}

void MediaPlayer::SetID3Info(MetaCategory category, QString info)
{
	if (info == NULL)
	{
		info = "";
	}
	if (category == MetaCategoryArtist)
	{
		emit artistChanged(info);
	}
	else if (category == MetaCategoryAlbum)
	{
		emit albumChanged(info);
	}
	else if (category == MetaCategoryGenre)
	{
		emit genreChanged(info);
	}
}

void MediaPlayer::SetAlbumArt(const unsigned char *buf, unsigned int length)
{
	bool decoded;

	decoded = _albumArt.loadFromData(buf, (int)length);
	if (decoded)
	{
		_albumArtPainter->SetAlbumArtImage(_albumArt);
		_albumArtBgPainter->SetAlbumArtImage(_albumArt);

		_noAlbumArt = false;
		emit noAlbumArtChanged();
	}
	else
	{
		fprintf(stderr, "%s: decode albumart failed\n", __PRETTY_FUNCTION__);
	}
}

void MediaPlayer::SetNoAlbumArt(void)
{
	_albumArt.load(":/audio/images/audio_noalbum.png");
	_albumArtPainter->SetAlbumArtImage(_albumArt);
	_albumArtBg.load(":/images/background_01.png");
	_albumArtBgPainter->SetAlbumArtImage(_albumArtBg);

	_noAlbumArt = true;
	emit noAlbumArtChanged();
}

void MediaPlayer::OnClickedRepeat()
{
	_userChangeMode = true;
	emit ChangeRepeatMode();
}

void MediaPlayer::OnClickedShuffle()
{
	_userChangeMode = true;
	emit ChangeShuffleMode();
}
