/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
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

