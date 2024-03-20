/****************************************************************************************
 *   FileName    : main.cpp
 *   Description : main.cpp
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

#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/signal.h>
#include <QTextCodec>
#include <QQuickItem>
#include <QQmlContext>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "MediaPlayer.h"
//#include "TCPopupManager.h"
#include "TMPlayerGUIIFManager.h"

static void NewExceptionHandler();
static void SignalHandler(int sig);
static void PrintQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);

QTextCodec *g_codecUtf16 = NULL;
QTextCodec *g_codecUtf8 = NULL;

int g_debug = 0;
bool g_v4lsink = true;
int g_appID = -1;
bool g_bt_ut = false;

MediaPlayer *_mediaPlayer = NULL;

int main(int argc, char *argv[])
{
	int ret = 0;

	qInstallMessageHandler(PrintQtMessage);

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	QObject* window;

	signal(SIGTERM, SignalHandler);

	QString argument;

	for (int i = 1; i < argc; i++)
	{
		argument = argv[i];
		argument = argument.toUpper();
		if (argument == "--NO-V4L2")
		{
			g_v4lsink = false;
		}
		else if (argument == "--DEBUG")
		{
			g_debug = 1;
		}
		else if (argument == "--BT_UT")
		{
			g_bt_ut = true;
		}
		else if (argument.left(5) == "--ID=")
		{
			g_appID = argument.mid(5).toInt();
		}
	}

	std::set_new_handler(NewExceptionHandler);
	g_codecUtf8 = QTextCodec::codecForName("UTF-8");
	g_codecUtf16 = QTextCodec::codecForName("UTF-16");

	ret = -1;

	qmlRegisterType<MediaPlayer>("MediaPlayer", 1, 0, "TCMediaPlayerWin");
	qmlRegisterType<AlbumArt>("MediaPlayer", 1, 0, "TCAlbumArt");

	engine.rootContext()->setContextProperty("PLAYER_WIDTH", PLAYER_WIDTH);
	engine.rootContext()->setContextProperty("PLAYER_HEIGHT", PLAYER_HEIGHT);
	engine.rootContext()->setContextProperty("OSD_SIZE", OSD_SIZE);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	window = engine.rootObjects().value(0);
	_mediaPlayer = qobject_cast<MediaPlayer *>(window);
	if (_mediaPlayer != NULL)
	{
		if (_mediaPlayer->Initialize())
		{
			ret = TM_PLAYER_GUI_IF_MANAGER->Initilaize();

			ret = app.exec();

			if (_mediaPlayer != NULL)
			{
				delete _mediaPlayer;
				_mediaPlayer = NULL;
			}
			TM_PLAYER_GUI_IF_MANAGER->Release();
		}
		else
		{
			fprintf(stderr, "tm player gui initialze failed\n");
		}
	}

	return ret;
}

static void NewExceptionHandler()
{
	fprintf(stderr, "[MASS PLAYER]FATAL ERROR : MEMORY ALLOCATION FAILED\n");
	throw std::bad_alloc();
}

static void SignalHandler(int sig)
{
	fprintf(stderr, "[MASS PLAYER] %s: received signal(%d)\n", __FUNCTION__, sig);
	QGuiApplication::exit(sig);
}

static void PrintQtMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	switch ((int)type)
	{
		case QtDebugMsg:
			fprintf(stderr, "QtDebug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtWarningMsg:
			fprintf(stderr, "QtWarning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "QtCritical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			break;
		case QtFatalMsg:
			fprintf(stderr, "QtFatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			abort();
	}
}
