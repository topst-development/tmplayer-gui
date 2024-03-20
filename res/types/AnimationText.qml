/****************************************************************************************
 *   FileName    : AnimationText.qml
 *   Description : QML script for animation text gui
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

Item {
	id: base

	property var text
	property var color
	property int size
/*
	property alias running: animation.running

	function restart() {
		animation.restart()
	}

	function pause() {
		animation.pause()
		aniText.x = 0
	}
*/
	clip: true

	TextMetrics {
		id: textMetrics

		text: base.text
		font.pixelSize: base.size
	}

	Text {
		id: aniText

		text: textMetrics.text
		color: base.color
		font: textMetrics.font

		verticalAlignment: Text.AlignVCenter
		renderType: Text.NativeRendering

		onTextChanged: {
			aniText.x = 0
		}
/*
		NumberAnimation on x {
			id: animation

			running: false
			from: 0
			to: -textMetrics.width * 1.5
			duration: 10000
			loops: Animation.Infinite
		}

		Text {
			x: textMetrics.width * 1.5

			text: aniText.text
			color: aniText.color
			font: aniText.font
			renderType: Text.NativeRendering
		}
*/
	}
}
