/****************************************************************************************
 *   FileName    : PopupList.qml
 *   Description : QML script for pop-up list gui
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
	id: popupList

	property var deviceName
	property bool hasVideo
	property bool isMeta
	property int requestItem: 1
	property int focusedItem: 1
	property int metaDepth: 0
	property var metaIndex: []

	signal backPressed

	function upPage() {
		mediaPlayerWin.OnClearList()

		if (!isMeta) {
			if (requestItem - 6 <= 0) {
				requestItem = parseInt((mediaPlayerWin.totalNumber - 1) / 6) * 6 + 1
			} else {
				requestItem = requestItem - 6
			}

			focusedItem = requestItem
			mediaPlayerWin.OnRequestFileList(requestItem, 6)
		} else {
			if (requestItem - 6 <= 0) {
				requestItem = parseInt((mediaPlayerWin.totalMetaNumber - 1) / 6) * 6 + 1
			} else {
				requestItem = requestItem - 6
			}

			focusedItem = requestItem
			mediaPlayerWin.OnRequestMetaList(requestItem)
		}
	}

	function downPage() {
		mediaPlayerWin.OnClearList()

		if (!isMeta) {
			if (requestItem + 6 <= mediaPlayerWin.totalNumber) {
				requestItem = requestItem + 6
			} else {
				requestItem = 1
			}

			focusedItem = requestItem
			mediaPlayerWin.OnRequestFileList(requestItem, 6)
		} else {
			if (requestItem + 6 <= mediaPlayerWin.totalMetaNumber) {
				requestItem = requestItem + 6
			} else {
				requestItem = 1
			}

			focusedItem = requestItem
			mediaPlayerWin.OnRequestMetaList(requestItem)
		}
	}

	function selectPrev() {
		if (focusedItem % 6 == 1) {
			popupList.upPage()
		} else {
			focusedItem--
		}
	}

	function selectNext() {
		if (focusedItem == mediaPlayerWin.totalNumber || focusedItem % 6 == 0) {
			popupList.downPage()
		} else {
			focusedItem++
		}
	}

	function selectItem() {
		console.log(focusedItem + " clicked")

		if (!isMeta) {
			mediaPlayerWin.OnListTrackNumberChanged(focusedItem)
		} else {
			mediaPlayerWin.OnSelectMetaList(focusedItem % 6)

			if (mediaPlayerWin.category == 5) {
				mediaPlayerWin.OnStartMetaList(6)
			}

			metaIndex[metaDepth] = requestItem
			metaDepth = metaDepth + 1
			requestItem = 1

			mediaPlayerWin.OnClearList()
			mediaPlayerWin.OnHomeMetaList(6)
		}

		popupList.backPressed()
	}

	onVisibleChanged: {
		if (this.visible) {
			//Show
			console.log("Show")
			if (!isMeta) {
				requestItem = parseInt((mediaPlayerWin.fileNumber - 1) / 6) * 6 + 1
				focusedItem = requestItem
				mediaPlayerWin.OnRequestFileList(requestItem, 6)
			} else {
				// Get Meta List Root
				mediaPlayerWin.OnStartMetaList(6)
			}
		} else {
			//Hide
			console.log("Hide")
			requestItem = 1
			focusedItem = 1
			mediaPlayerWin.OnClearList()

			if (isMeta) {
				metaDepth = 0
				metaIndex = []
				mediaPlayerWin.OnEndMetaList(1)
			}
		}
	}

	Image {
		id: background

		anchors.fill: parent

		source: "/images/background_02.png"

		Item {

			x: (parent.width - PLAYER_WIDTH) / 2
			y: 0
			width: PLAYER_WIDTH
			height: parent.height

			Image {
				id: iconTitle

				x: 42
				y: 50
				width: 70
				height: 44

				source: "/icon/images/browser_title_folder.png"
			}

			Text {
				id: title

				x: 148
				y: 50
				width: 320
				height: 45

				color: "white"
				text: isMeta ? mediaPlayerWin.metaMenu : "Home"

				renderType: Text.NativeRendering
				font.pixelSize: 25
			}

			Text {
				id: indexInfo

				x: 720
				y: 50
				width: 256
				height: 62

				color: "white"
				text: {
					if (!isMeta) {
						var currentPage = parseInt((requestItem - 1) / 6) + 1
						var totalPages = parseInt((mediaPlayerWin.totalNumber - 1) / 6) + 1
					} else {
						var currentPage = parseInt((requestItem - 1) / 6) + 1
						var totalPages = parseInt((mediaPlayerWin.totalMetaNumber - 1) / 6) + 1
					}

					return currentPage + "/" + totalPages
				}

				renderType: Text.NativeRendering
				font.pixelSize: 20
				horizontalAlignment: Text.AlignRight
			}

			Item {
				id: table

				x: 42
				y: 96
				width: 869
				height: 348

				ListView {
					id: list

					anchors.fill: parent

					spacing: 1

					model: mediaPlayerWin.list

					delegate: ImageButton {
						id: listItem

						width: 869
						height: 58

						source: {
							if (focusedItem == indexNumber) {
								return "/browser/images/browser_list_bg_03.png"
							} else {
								if (index % 2 == 0) {
									return "/browser/images/browser_list_bg_01.png"
								} else {
									return "/browser/images/browser_list_bg_02.png"
								}
							}
						}

						onClicked: {
							console.log(index + " clicked")

							if (!isMeta) {
								focusedItem = indexNumber
								mediaPlayerWin.OnListTrackNumberChanged(indexNumber)
							} else {
								mediaPlayerWin.OnSelectMetaList(index + 1)

								if (mediaPlayerWin.category == 5) {
									mediaPlayerWin.OnStartMetaList(6)
								}

								metaIndex[metaDepth] = requestItem
								metaDepth = metaDepth + 1
								requestItem = 1

								mediaPlayerWin.OnClearList()
								mediaPlayerWin.OnHomeMetaList(6)
							}

							popupList.backPressed()
						}

						Image {
							id: listIcon

							width: 70
							height: 44
							anchors.verticalCenter: listItem.verticalCenter

							source: {
								if (!isMeta) {
									if (mediaPlayerWin.fileNumber == indexNumber) {
										return "/icon/images/browser_list_playing_n.png"
									} else {
										return "/icon/images/browser_list_file_n.png"
									}
								} else if (mediaPlayerWin.category == 0) {
									if (indexNumber == 2) {
										return "/icon/images/browser_list_artist_n.png"
									} else if (indexNumber == 3) {
										return "/icon/images/browser_list_album_n.png"
									} else if (indexNumber == 4) {
										return "/icon/images/browser_list_genre_n.png"
									} else if (indexNumber == 5) {
										return "/icon/images/browser_list_song_n.png"
									} else if (indexNumber == 6) {
										return "/icon/images/browser_list_composer_n.png"
									} else if (indexNumber == 7) {
										return "/icon/images/browser_list_audiobook_n.png"
									} else if (indexNumber == 8) {
										return "/icon/images/browser_list_podcast_n.png"
									} else if (indexNumber == 9) {
										return "/icon/images/browser_list_radio_n.png"
									} else {
										return "/icon/images/browser_list_playlist_n.png"
									}
								} else if (mediaPlayerWin.category == 2) {
									return "/icon/images/browser_list_artist_n.png"
								} else if (mediaPlayerWin.category == 3) {
									return "/icon/images/browser_list_album_n.png"
								} else if (mediaPlayerWin.category == 4) {
									return "/icon/images/browser_list_genre_n.png"
								} else if (mediaPlayerWin.category == 5) {
									return "/icon/images/browser_list_song_n.png"
								} else if (mediaPlayerWin.category == 6) {
									return "/icon/images/browser_list_composer_n.png"
								} else if (mediaPlayerWin.category == 7) {
									return "/icon/images/browser_list_audiobook_n.png"
								} else if (mediaPlayerWin.category == 8) {
									return "/icon/images/browser_list_podcast_n.png"
								} else if (mediaPlayerWin.category == 9) {
									return "/icon/images/browser_list_radio_n.png"
								} else {
									return "/icon/images/browser_list_playlist_n.png"
								}
							}
						}

						Text {
							id: listTitle

							width: 750
							height: 58

							anchors.left: listIcon.right

							color: "white"
							text: fileName
							elide: Text.ElideRight
							verticalAlignment: Text.AlignVCenter
							renderType: Text.NativeRendering
							font.pixelSize: 25
						}
					}
				}
			}

			ImageButton {
				id: upButton

				x: 913
				y: 97
				width: 69
				height: 114

				source: pressed ? "/browser/images/browser_scroll_btn_up_p.png" : "/browser/images/browser_scroll_btn_up_n.png"

				onClicked: {
					popupList.upPage()
				}
			}

			ImageButton {
				id: downButton

				x: 913
				y: 334
				width: 69
				height: 114

				source: pressed ? "/browser/images/browser_scroll_btn_down_p.png" : "/browser/images/browser_scroll_btn_down_n.png"

				onClicked: {
					popupList.downPage()
				}
			}

			ImageButton {
				id: backButton

				x: 41
				y: 453
				width: 314
				height: 74

				source: pressed ? "/browser/images/browser_toolbox_03_back_btn_p.png" : "/browser/images/browser_toolbox_03_back_btn_n.png"

				onClicked: {
					popupList.backPressed()
				}
			}

			ImageButton {
				id: homeButton

				x: 355
				y: 453
				width:314
				height: 74

				visible: isMeta
				source: pressed ? "/browser/images/browser_toolbox_03_home_btn_p.png" : "/browser/images/browser_toolbox_03_home_btn_n.png"

				onClicked: {
					mediaPlayerWin.OnClearList()
					mediaPlayerWin.OnHomeMetaList(6)

					requestItem = 1
				}
			}

			ImageButton {
				id: undoButton

				x: 669
				y: 453
				width:314
				height: 74

				visible: isMeta
				source: {
					if (mediaPlayerWin.category == 0) {
						return "/browser/images/browser_toolbox_03_up_btn_d.png"
					} else {
						return pressed ? "/browser/images/browser_toolbox_03_up_btn_p.png" : "/browser/images/browser_toolbox_03_up_btn_n.png"
					}
				}

				onClicked: {
					mediaPlayerWin.OnClearList()
					mediaPlayerWin.OnUndoMetaList()

					metaIndex[metaDepth] = 0
					metaDepth = metaDepth - 1
					requestItem = metaIndex[metaDepth]
				}
			}
		}
	}
}
