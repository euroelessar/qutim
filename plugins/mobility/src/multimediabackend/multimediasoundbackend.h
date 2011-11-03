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
#include <QAudioOutput>
#include <QThread>

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
};

class MultimediaSound : public QObject
{
    Q_OBJECT
public:
    MultimediaSound(const QString &filename, QObject *parent = 0);
public slots:
    void play();
    void finishedPlaying(QAudio::State state);
private:
    QString m_filename;
};
}

#endif // MULTIMEDIASOUNDBACKEND_H
