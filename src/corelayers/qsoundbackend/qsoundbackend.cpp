/****************************************************************************
 *  qsoundbackend.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "qsoundbackend.h"
#include <QSound>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include "modulemanagerimpl.h"

static Core::CoreModuleHelper<QSoundBackend> sound_theme_static(
		QT_TRANSLATE_NOOP("Plugin", "Default sound engine"),
		QT_TRANSLATE_NOOP("Plugin", "Sound engine based on QSound")
		);

void QSoundBackend::playSound(const QString &filename)
{
	// FIXME: Possibility of freeze at Ubuntu is there
	// May be we should run it at non-gui thread firstly?
	if (!QSound::isAvailable()) {
		qWarning() << "QSound: Unable to play sound";
	}
	QSound(filename).play();
}

QStringList QSoundBackend::supportedFormats()
{
	//TODO
	return QStringList() << "wav";
}
