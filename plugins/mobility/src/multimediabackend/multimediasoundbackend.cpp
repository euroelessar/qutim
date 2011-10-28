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

#include "multimediasoundbackend.h"
#include <QAudioDeviceInfo>
#include <QFile>
#include <QDataStream>
#include <QtEndian>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

namespace Core
{
MultimediaSoundBackend::MultimediaSoundBackend()
{
}

void MultimediaSoundBackend::playSound(const QString &filename)
{
    QThread *t = new QThread();
    MultimediaSound *task = new MultimediaSound(filename);
    task->moveToThread(t);
    t->start(QThread::LowPriority);
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(task, SIGNAL(destroyed()), t, SLOT(quit()));
    QTimer::singleShot(0, task, SLOT(play()));
}

QStringList MultimediaSoundBackend::supportedFormats()
{
    return QStringList() << "wav"; //WTF Oo
}

MultimediaSound::MultimediaSound(const QString &filename, QObject *parent) :
    QObject(parent),
    m_filename(filename)
{
}

void MultimediaSound::play()
{
    QScopedPointer<QFile> file(new QFile(m_filename));
    if (file->size() < 8 || !file->open(QIODevice::ReadOnly))
        return;

    quint32 RIFF_str = 0x46464952;
    quint32 WAVE_str = 0x45564157;
    quint32 fmt_str  = 0x20746D66;
    quint32 data_str = 0x61746164;

    quint32 riffId, riffLength, waveId, waveFmt, waveLength;
    quint16 type, channels, align, bitsPerSample;
    quint32 frequency, bytesPerSec;
    quint32 dataId, dataSize;

    {
        QDataStream in(file.data());
        in.setByteOrder(QDataStream::LittleEndian);

        in >> riffId >> riffLength >> waveId >> waveFmt >> waveLength;
        if (riffId != RIFF_str || waveId != WAVE_str || waveFmt != fmt_str) {
            qWarning() << m_filename << "is not valid WAV file";
            return;
        }

        in >> type >> channels >> frequency >> bytesPerSec >> align >> bitsPerSample;
        in.skipRawData(waveLength - 16);
        if (type != 1) {
            qWarning("Unsupported WAV compression type: 0x%s", qPrintable(QString::number(type, 16)));
            return;
        }
        in >> dataId >> dataSize;
        if (dataId != data_str) {
            qWarning() << m_filename << "is not valid WAV file";
            return;
        }
    }

    QAudioFormat format;
    format.setChannels(channels);
    format.setFrequency(frequency);
    format.setSampleSize(bitsPerSample);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(bitsPerSample == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
    format.setCodec("audio/pcm");
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

    if (!info.isFormatSupported(format)) {
        qWarning() << "Audio format not supported by backend, cannot play audio";
        return;
    }
    QAudioOutput *audio = new QAudioOutput(info, format, this);
    QEventLoop loop;
    file->setParent(audio);
    connect(audio, SIGNAL(stateChanged(QAudio::State)), SLOT(finishedPlaying(QAudio::State)));
    audio->start(file.take());
    loop.exec();
}

void MultimediaSound::finishedPlaying(QAudio::State state)
{   
    if (state == QAudio::IdleState) {
        QAudioOutput *audio = qobject_cast<QAudioOutput*>(sender());
        Q_ASSERT(audio);
        audio->deleteLater();
        QTimer::singleShot(0, this, SLOT(quit()));
    }
}
}
