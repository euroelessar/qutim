/****************************************************************************
 *  qsoundbackend.h
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

#ifndef QSOUNDBACKEND_H
#define QSOUNDBACKEND_H
#include <qutim/notificationslayer.h>

using namespace qutim_sdk_0_3;

class QSoundBackend : public SoundBackend
{
public:
	virtual void playSound(const QString& filename);
    virtual QStringList supportedFormats();
};

#endif // QSOUNDBACKEND_H
