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

#include <qutim/notificationslayer.h>
#include <QAudioOutput>

using namespace qutim_sdk_0_3;

namespace Core
{
class MultimediaSoundBackend : public SoundBackend
{
Q_OBJECT
public:
    MultimediaSoundBackend();
	virtual void playSound(const QString &filename);
	virtual QStringList supportedFormats();
public slots:
	void finishedPlaying(QAudio::State state);
};
}

#endif // MULTIMEDIASOUNDBACKEND_H
