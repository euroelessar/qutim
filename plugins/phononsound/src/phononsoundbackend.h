/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef MULTIMEDIASOUNDBACKEND_H
#define MULTIMEDIASOUNDBACKEND_H

#include <qutim/sound.h>
#include <phonon/backendcapabilities.h>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <QStringList>

using namespace qutim_sdk_0_3;

class PhononSoundBackend : public SoundBackend
{
	Q_OBJECT
public:
    PhononSoundBackend();
	virtual void playSound(const QString &filename);
	virtual QStringList supportedFormats();
public slots:
	void finishedPlaying();
private:
	struct PathData
	{
		Phonon::MediaObject *mediaObject;
		Phonon::AudioOutput *audioOutput;
		Phonon::Path path;
	};

	QMap<QObject*, PathData> m_cache;
	QStringList m_formats;
};

#endif // MULTIMEDIASOUNDBACKEND_H
