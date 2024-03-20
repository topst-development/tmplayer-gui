/****************************************************************************************
 *   FileName    : VideoPlayer.qml
 *   Description : QML script for video player gui
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
	id: videoPlayer
	property bool fullScreen: false

	signal listPressed

	onVisibleChanged: {
		if (!this.visible) {
			fileName.pause()
		}
	}

	MouseArea {
		id: background
		anchors.fill: parent
		onClicked: {
			console.log("background Clicked\n")
			fullScreen = !fullScreen
			if (fullScreen)
			{
				controller.visible = false
			}
			else
			{
				controller.visible = true
			}
			//emit ShowVideoFullScreen(_fullScreen);
		}

		Image {
			id: controller
			x: 0
			width: 1024
			height: 174

			anchors.bottom: parent.bottom
			source: "video/images/video_bg_30.png"

			Slider {
				id: sliderTime
				x: 125
				y: 66
				width: 771
				height: 39
				value: 0
				onValueChangedByHandle:{
					if (!pressed) {
						mediaPlayerWin.OnSliderValueChanged(newValue);
					}
				}
			}

			ImageButton {
				id: btnContentChange
				x: 43
				y: 12
				width: 171
				height: 47
				source: pressed ? "images/content_change_btn_p.png" : "images/content_change_btn_n.png"
				onClicked: {
					console.log("btnContentChange Clicked\n")
					mediaPlayerWin.OnChangeContent();
				}
			}

			ImageButton {
				id: btnTrackPlay
				x: 472
				y: 100
				width: 80
				height: 80
				source: pressed ? "video/images/video_play_btn_p.png" : "video/images/video_play_btn_n.png"
				onClicked: {
					console.log("btnTrackPlay Clicked\n")
					mediaPlayerWin.OnClickedPlayResume();
				}
			}

			ImageButton {
				id: btnTrackPause
				x: 472
				y: 100
				width: 80
				height: 80
				source: pressed ? "video/images/video_pause_btn_p.png" : "video/images/video_pause_btn_n.png"
				visible: false
				onClicked: {
					console.log("btnTrackPause Clicked\n")
					mediaPlayerWin.OnClickedPlayPause();
				}
			}

			ImageButton {
				id: btnTrackFF
				x: 674
				y: 100
				width: 80
				height: 80
				source: pressed ? "video/images/video_ff_btn_p.png" : "video/images/video_ff_btn_n.png"
				onClicked: {
					console.log("btnTrackFF Clicked\n")
					mediaPlayerWin.OnClickedTrackUp();
				}
			}

			ImageButton {
				id: btnTrackREW
				x: 270
				y: 100
				width: 80
				height: 80
				source: pressed ? "video/images/video_rew_btn_p.png" : "video/images/video_rew_btn_n.png"
				onClicked: {
					console.log("btnTrackREW Clicked\n")
					mediaPlayerWin.OnClickedTrackDown();
				}
			}

			ImageButton {
				id: btnList
				x: 933
				y: 12
				width: 50
				height: 47
				source: pressed ? "video/images/video_list_btn_p.png" : "video/images/video_list_btn_n.png"
				onClicked: {
					console.log("btnList Clicked\n")
					videoPlayer.listPressed()
				}
			}

			Text {
				id: playTime
				x: 0
				y: 74
				width: 121
				height: 22
				horizontalAlignment: Text.AlignRight
				text: mediaPlayerWin.playTimeString
				color: "#FF70C1FF"
				font.pixelSize: 16
				renderType: Text.NativeRendering
			}

			Text {
				id: totalTime
				x: 902
				y: 74
				width: 90
				height: 22
				horizontalAlignment: Text.AlignRight
				text: mediaPlayerWin.totalTimeString
				color: "#AAAAAA"
				font.pixelSize: 16
				renderType: Text.NativeRendering
			}

			AnimationText {
				id: fileName
				x: 239
				y: 12
				width: 680
				height: 47
				text: mediaPlayerWin.fileNameString
				color: "white"
				size: 30
			}
		}
	}

	Connections{
		target:mediaPlayerWin

		onSetVideoPlayTime:{
			playTime.text = time;
			sliderTime.value = value;
		}
		onSetVideoTotalTime:{
			totalTime.text = time;
			sliderTime.maximum = maxValue;
		}
		onSetVideoFilename:{
			fileName.text = text;
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
