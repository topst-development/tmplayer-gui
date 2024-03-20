/****************************************************************************************
 *   FileName    : AudioPlayer.qml
 *   Description : QML script for audio player gui
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
No warranty is made, express or implied, regarding the information’s accuracy,
completeness, or performance.
In no event shall Telechips be liable for any claim, damages or other liability arising from,
out of or in connection with this source code or the use in the source code.
This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement
between Telechips and Company.
*
****************************************************************************************/

import QtQuick 2.6
import MediaPlayer 1.0
import "types"

Item {
	id: audioPlayer

	property bool hasVideo

	signal listPressed

	onVisibleChanged: {
		if (!this.visible) {
			fileName.pause()
		}
	}

	TCAlbumArt {
		id: background
		objectName: "albumArtBg"

		anchors.fill: parent

		Image {
			id: albumArtMask
			anchors.fill: parent

			visible: mediaPlayerWin.noAlbumArt == false

			source: "/audio/images/audio_bg_mask.png"
		}

		TCAlbumArt {
			id: albumArt
			objectName: "albumArt"

			x: 53
			y: 80
			width: 334
			height: 333
		}

		Image {
			id: albumIcon
			x: 424
			y: 110
			width: 40
			height: 40
			source: "icon/images/audio_info_album.png"
		}

		Image {
			id: artistIcon
			x: 424
			y: 150
			width: 40
			height: 40
			source: "icon/images/audio_info_artist.png"
		}

		Image {
			id: genreIcon
			x: 424
			y: 190
			width: 40
			height: 40
			source: "icon/images/audio_info_genre.png"
		}

		Image {
			id: titleIcon
			x: 424
			y: 230
			width: 40
			height: 40
			source: "icon/images/audio_info_title.png"
		}

		Image {
			id: shffleIcon
			x: 635
			y: 22
			width: 50
			height: 35
			source: "icon/images/audio_playmode_shuffle_on.png"
			visible: mediaPlayerWin.shuffle ? true : false
		}

		Image {
			id: repeatIcon
			x: 705
			y: 22
			width: 50
			height: 35
			source: {
				if (mediaPlayerWin.repeat == 0) {
					return ""
				} else if (mediaPlayerWin.repeat == 1) {
					return "icon/images/audio_playmode_repeat_track.png"
				} else if (mediaPlayerWin.repeat == 3) {
					return "icon/images/audio_playmode_repeat_all.png"
				} else {
					return ""
				}
			}
		}

		ImageButton {
			id: btnContentChange
			x: 424
			y: 66
			width: 171
			height: 47
			source: pressed ? "images/content_change_btn_p.png" : "images/content_change_btn_n.png"
			visible: hasVideo
			onClicked: {
				console.log("btnContentChange Clicked\n")
				mediaPlayerWin.OnChangeContent();
			}
		}

		ImageButton {
			id: btnTrackPlay
			x: 667
			y: 322
			width: 80
			height: 80
			source: pressed ? "audio/images/audio_play_btn_p.png" : "audio/images/audio_play_btn_n.png"
			onClicked: {
				console.log("btnTrackPlay Clicked\n")
				mediaPlayerWin.OnClickedPlayResume();
			}
		}

		ImageButton {
			id: btnTrackPause
			x: 667
			y: 322
			width: 80
			height: 80
			source: pressed ? "audio/images/audio_pause_btn_p.png" : "audio/images/audio_pause_btn_n.png"
			visible: false
			onClicked: {
				console.log("btnTrackPaused Clicked\n")
				mediaPlayerWin.OnClickedPlayPause();
			}
		}

		ImageButton {
			id: btnTrackFF
			x: 838
			y: 322
			width: 80
			height: 80
			source: pressed ? "audio/images/audio_ff_btn_p.png" : "audio/images/audio_ff_btn_n.png"
			onClicked: {
				console.log("btnTrackFF Clicked\n")
				mediaPlayerWin.OnClickedTrackUp();
			}
		}

		ImageButton {
			id: btnTrackREW
			x: 495
			y: 322
			width: 80
			height: 80
			source: pressed ? "audio/images/audio_rew_btn_p.png" : "audio/images/audio_rew_btn_n.png"
			onClicked: {
				console.log("btnTrackREW Clicked\n")
				mediaPlayerWin.OnClickedTrackDown();
			}
		}

		Slider {
			id: sliderTime
			x: 425
			y: 285
			width: 563
			height: 39
			value: 0
			onValueChangedByHandle:{
				if (!pressed) {
					mediaPlayerWin.OnSliderValueChanged(newValue);
				}
			}
		}

		ImageButton {
			id: btnSuffle
			x: 41
			y: 440
			width: 314
			height: 74
			source: pressed ? "audio/images/audio_toolbox_04_shuffle_btn_p.png" : "audio/images/audio_toolbox_04_shuffle_btn_n.png"
			onClicked: {
				console.log("btnShuffle Clicked\n")
				mediaPlayerWin.OnClickedShuffle();
			}
		}

		ImageButton {
			id: btnRepeat
			x: 355
			y: 440
			width: 314
			height: 74
			source: pressed ? "audio/images/audio_toolbox_04_repeat_btn_p.png" : "audio/images/audio_toolbox_04_repeat_btn_n.png"
			onClicked: {
				console.log("btnRepeat Clicked\n")
				mediaPlayerWin.OnClickedRepeat();
			}
		}
		ImageButton {
			id: btnList
			x: 669
			y: 440
			width: 314
			height: 74
			source: pressed ? "audio/images/audio_toolbox_04_list_btn_p.png" : "audio/images/audio_toolbox_04_list_btn_n.png"
			onClicked: {
				console.log("btnList Clicked\n")
				audioPlayer.listPressed()
			}
		}

		Text {
			id: playTime
			x: 425
			y: 272
			width: 102
			height: 22
			text: mediaPlayerWin.playTimeString
			color: "#FF70C1FF"
			font.pixelSize: 16
			renderType: Text.NativeRendering
		}

		Text {
			id: totalTime
			x: 886
			y: 272
			width: 102
			height: 22
			horizontalAlignment: Text.AlignRight
			text: mediaPlayerWin.totalTimeString
			color: "#AAAAAA"
			font.pixelSize: 16
			renderType: Text.NativeRendering
		}

		Text {
			id: fileNumber
			x: 808
			y: 22
			width: 180
			height: 43
			horizontalAlignment: Text.AlignRight
			text: "1 / 1"
			color: "#AAAAAA"
			font.pixelSize: 22
			renderType: Text.NativeRendering
		}


		Text {
			id: albumName
			x: 464
			y: 110
			width: 418
			height: 25
			horizontalAlignment: Text.AlignLeft
			text: "No Album"
			color: "white"
			elide: Text.ElideRight
			font.pixelSize: 22
			renderType: Text.NativeRendering
		}

		Text {
			id: artistName
			x: 465
			y: 150
			width: 418
			height: 25
			horizontalAlignment: Text.AlignLeft
			text: "No Artist"
			color: "white"
			elide: Text.ElideRight
			font.pixelSize: 22
			renderType: Text.NativeRendering
		}

		Text {
			id: genreName
			x: 465
			y: 190
			width: 418
			height: 25
			horizontalAlignment: Text.AlignLeft
			text: "No Genre"
			color: "white"
			elide: Text.ElideRight
			font.pixelSize: 22
			renderType: Text.NativeRendering
		}

		AnimationText {
			id: fileName
			x: 465
			y: 230
			width: 520
			height: 35
			text: mediaPlayerWin.fileNameString
			color: "white"
			size: 22
		}

	}
	Connections{
		target:mediaPlayerWin

		onSetAudioPlayTime:{
			playTime.text = time;
			sliderTime.value = value;
		}
		onSetAudioTotalTime:{
			totalTime.text = time;
			sliderTime.maximum = maxValue;
		}
		onSetAudioFilename:{
			fileName.text = text;
		}
		onArtistChanged:{
			artistName.text = artistText;
		}
		onAlbumChanged:{
			albumName.text = albumText;
		}
		onGenreChanged:{
			genreName.text = genreText;
		}
		onFileIndexChanged:{
			fileNumber.text = index;
		}
		onSetPlayStatus: {
			if (play) {
				btnTrackPlay.visible = false;
				btnTrackPause.visible = true;

				fileName.restart()
			} else {
				btnTrackPlay.visible = true;
				btnTrackPause.visible = false;

				fileName.pause()
			}
		}
	}
}
