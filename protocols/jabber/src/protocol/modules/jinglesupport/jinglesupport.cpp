/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

//#ifdef JABBER_HAVE_MULTIMEDIA

#include "jinglesupport.h"
#include <jreen/jinglesession.h>
#include <QAudioInput>
#include <QAudioOutput>
#include <QDebug>

namespace Jabber
{

JingleHelper::JingleHelper(Jreen::JingleAudioContent *content)
    : QObject(content)
{
	qDebug() << Q_FUNC_INFO;
	QIODevice *device = content->audioDevice();
	Q_ASSERT(device->isOpen());
	const Jreen::JingleAudioPayload payload = content->currentPayload();
	QAudioFormat format;
	format.setByteOrder(static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder));
	format.setChannelCount(payload.channelCount());
	format.setCodec(QLatin1String("audio/pcm"));
	format.setSampleType(QAudioFormat::SignedInt);
	format.setSampleSize(16);
	format.setSampleRate(payload.clockRate());
	
	const int bufferSize = payload.clockRate() * payload.channelCount() * 2 * 160 / 1000;
//	const int bufferSize = content->currentPayloadFrameSize();
	
	QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
	QAudioInput *input = new QAudioInput(info, format, this);
	input->setBufferSize(bufferSize);
	input->start(device);
	
	info = QAudioDeviceInfo::defaultOutputDevice();
	QAudioOutput *output = new QAudioOutput(info, format, this);
	output->setBufferSize(bufferSize);
	output->start(device);
}

JingleSupport::JingleSupport() : m_client(0)
{
	qDebug() << Q_FUNC_INFO;
}

void JingleSupport::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
{
	qDebug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	m_client = params.item<Jreen::Client>();
	connect(m_client->jingleManager(), SIGNAL(sessionCreated(Jreen::JingleSession*)),
	        SLOT(onSessionCreated(Jreen::JingleSession*)));
}

void JingleSupport::onSessionCreated(Jreen::JingleSession *session)
{
	qDebug() << Q_FUNC_INFO << session;
	connect(session, SIGNAL(contentAdded(Jreen::JingleContent*)),
	        SLOT(onContentAdded(Jreen::JingleContent*)));
	foreach (const QString &name, session->contents())
		onContentAdded(session->content(name));
}

void JingleSupport::onContentAdded(Jreen::JingleContent *content)
{
	qDebug() << Q_FUNC_INFO << content;
	if (!qobject_cast<Jreen::JingleAudioContent*>(content))
		return;

	connect(content, SIGNAL(stateChanged(Jreen::JingleContent::State)),
	        SLOT(onStateChanged(Jreen::JingleContent::State)));
}

void JingleSupport::onStateChanged(Jreen::JingleContent::State state)
{
	Jreen::JingleAudioContent *content = qobject_cast<Jreen::JingleAudioContent*>(sender());
	Q_ASSERT(content);
	qDebug() << Q_FUNC_INFO << state;
	if (state == Jreen::JingleContent::Connected && !m_helpers.contains(content))
		m_helpers.insert(content, new JingleHelper(content));
}

}

//#endif
