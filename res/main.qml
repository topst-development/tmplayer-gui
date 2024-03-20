/****************************************************************************************
 *   FileName    : main.qml
 *   Description : QML script for main window
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
import QtQuick.Window 2.2
import MediaPlayer 1.0
import "types"

TCMediaPlayerWin {
	id: mediaPlayerWin

	x: 0
	y: (Screen.height - PLAYER_HEIGHT) / 2 + OSD_SIZE
	width: Screen.width
	height: Screen.height - ((Screen.height - PLAYER_HEIGHT) / 2 + OSD_SIZE)

	flags: Qt.FramelessWindowHint

	title: qsTr("Telechips Media Player")
	visible: false
	color: "transparent"

	onBackPressed: {
		if (popupList.visible) {
			popupList.visible = false
		} else {
			mediaPlayerWin.goBack()
		}
	}

	onUpPressed: {
		if (popupList.visible) {
			popupList.upPage()
		}
	}

	onDownPressed: {
		if (popupList.visible) {
			popupList.downPage()
		}
	}

	onPrevPressed: {
		if (popupList.visible) {
			popupList.selectPrev()
		} else {
			mediaPlayerWin.OnClickedTrackDown()
		}
	}

	onNextPressed: {
		if (popupList.visible) {
			popupList.selectNext()
		} else {
			mediaPlayerWin.OnClickedTrackUp()
		}
	}

	onOkPressed: {
		if (popupList.visible) {
			popupList.selectItem()
		} else {
			if (mediaPlayerWin.playStatus) {
				mediaPlayerWin.OnClickedPlayPause()
			} else {
				mediaPlayerWin.OnClickedPlayResume()
			}
		}
	}

	onRepeatPressed: {
		if (audioPlayer.visible) {
			mediaPlayerWin.OnClickedRepeat()
		}
	}

	onShufflePressed: {
		if (audioPlayer.visible) {
			mediaPlayerWin.OnClickedShuffle()
		}
	}

	onChangedDevice: {
		console.log("Device Changed\n")

		if (popupList.visible) {
			popupList.visible = false
		}

		if (mediaPlayerWin.device == 5) {
			//show AUX Player
			auxPlayer.visible = true
		} else if ((mediaPlayerWin.device >= 1) && (mediaPlayerWin.device <= 7)) {
			//show Audio/Video Player
			//hide AUX Player
			auxPlayer.visible = false
		} else {
			//No Device
			auxPlayer.visible = false
		}
	}

	onChangedContent: {
		console.log("content Changed\n")

		if (popupList.visible) {
			popupList.visible = false
		}

		if (mediaPlayerWin.device != 5) {
			if (mediaPlayerWin.content == 0) {
				//show Audio Player
				videoPlayer.visible = false
				audioPlayer.visible = true
			} else if (mediaPlayerWin.content == 1) {
				//show Video Player
				audioPlayer.visible = false
				videoPlayer.visible = true
			} else {
				//No Content
			}
		}
	}

	Item {
		id: playerArea

		x: (Screen.width - PLAYER_WIDTH) / 2
		y: 0
		width: PLAYER_WIDTH
		height: mediaPlayerWin.height

		AudioPlayer {
		    id: audioPlayer
		    objectName: "audioPlayer"
		    anchors.fill: parent

			hasVideo: (device <= 4) //Storage Device

			onListPressed: {
				popupList.visible = true
			}
		}

		VideoPlayer {
			id: videoPlayer
			objectName: "videoPlayer"
			anchors.fill: parent

			onListPressed: {
				popupList.visible = true
			}
		}

		AuxPlayer {
			id: auxPlayer
			objectName: "auxPlayer"

			anchors.fill: parent
			visible: false
		}

		ImageButton {
			id: deviceIcon

			x: 53
			y: 20
			width: 130
			height: 37

			source: {
				if (mediaPlayerWin.device == 1) {
					return "/images/top_title_usb_n.png"
				} else if (mediaPlayerWin.device == 2) {
					return "/images/top_title_usb_02_n.png"
				} else if (mediaPlayerWin.device == 3) {
					return "/images/top_title_usb_03_n.png"
				} else if (mediaPlayerWin.device == 4) {
					return "/images/top_title_sdmmc_n.png"
				} else if (mediaPlayerWin.device == 5) {
					return "/images/top_title_aux_n.png"
				} else if (mediaPlayerWin.device == 6) {
					return "/images/top_title_ipod.png"
				} else if (mediaPlayerWin.device == 7) {
					return "/images/top_title_bt_n.png"
				} else {
					return ""
				}
			}

			onClicked: {
				mediaPlayerWin.OnChangeDeviceSource()
			}
		}
	}

	PopupList {
		id: popupList

		x: 0
		y: 0
		width: mediaPlayerWin.width
		height: mediaPlayerWin.height

		visible: false

		deviceName: device
		hasVideo: (device <= 4) //Storage Device
		isMeta: (device == 6) //iPod

		onBackPressed: {
			popupList.visible = false
		}
	}
}
