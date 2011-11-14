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

#include "soundenginelayer.h"
#include <QTimer>

SoundEngineLayerImpl::SoundEngineLayerImpl()
{
//	QList<AudioOutputDevice> devices = BackendCapabilities::availableAudioOutputDevices();
//	foreach( AudioOutputDevice device, devices )
//	{
//		qDebug() << device.description() << device.propertyNames();
//	}
//	QList<Phonon::AudioCaptureDevice> devices = Phonon::BackendCapabilities::availableAudioCaptureDevices();
//	foreach(Phonon::AudioCaptureDevice device, devices)
//	{
//		m_capture.setAudioCaptureDevice(device);
//	}
//	qDebug() << m_capture.audioCaptureDevice().name();
//	Experimental::AudioDataOutput *output = new Experimental::AudioDataOutput;
//	output->setFormat(Experimental::AudioDataOutput::IntegerFormat);
//	connect(output, SIGNAL(dataReady(QMap<Phonon::Experimental::AudioDataOutput::Channel,QVector<qint16> >)),
//			this, SLOT(dataReady(QMap<Phonon::Experimental::AudioDataOutput::Channel,QVector<qint16> >)));
//	createPath(&m_capture, output);
//	m_capture.start();
//	QTimer::singleShot(10000, this, SLOT(stopRecord()));
}

 bool SoundEngineLayerImpl::init(PluginSystemInterface *plugin_system)
 {
	 Q_UNUSED(plugin_system);
	 return true;
 }

 void SoundEngineLayerImpl::setProfileName(const QString &profile_name)
 {
	 m_profile_name = profile_name;
 }

void SoundEngineLayerImpl::playSound(QIODevice *device)
{
	MediaSource source(device);
	playSound(source);
}

void SoundEngineLayerImpl::playSound(const QString &filename)
{
	MediaSource source(filename);
	playSound(source);
}

void SoundEngineLayerImpl::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("phonon");
	settings.endGroup();
}

void SoundEngineLayerImpl::playSound(const MediaSource &source)
{
	MediaObject *object = new MediaObject(this);
	object->setCurrentSource(source);
	AudioOutput *output = new AudioOutput(object);
	Path path = createPath(object, output);
	connect(object, SIGNAL(finished()), output, SLOT(deleteLater()));
	connect(object, SIGNAL(finished()), object, SLOT(deleteLater()));
	object->play();
}

QIODevice *SoundEngineLayerImpl::grabSound()
{
    return 0;
}

void SoundEngineLayerImpl::killDevice()
{
//    dynamic_cast<Phonon::MediaObject *>(sender())->stop();
//	delete sender();
}

void SoundEngineLayerImpl::stopRecord()
{
//	qDebug() << "BUFFER" << m_buffer.size();
}

//void SoundEngineLayerImpl::dataReady(const QMap<Experimental::AudioDataOutput::Channel, QVector<qint16> > &data)
//{
//	qDebug() << data.size();
//}

