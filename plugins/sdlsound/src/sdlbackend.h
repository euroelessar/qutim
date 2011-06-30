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

#ifndef SDLSOUNDBACKEND_H
#define SDLSOUNDBACKEND_H

#include <qutim/sound.h>
#include <QStringList>
#include <QCache>

using namespace qutim_sdk_0_3;

class SDLSoundData;

class SDLSoundBackend : public SoundBackend
{
	Q_OBJECT
public:
    SDLSoundBackend();
	virtual ~SDLSoundBackend();
	virtual void playSound(const QString &filename);
	virtual QStringList supportedFormats();

	static void channelFinished(int channel);
private:
	QCache<QString, SDLSoundData> m_cache;
};

#endif // SDLSOUNDBACKEND_H
