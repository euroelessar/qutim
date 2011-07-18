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
#include "../../account/roster/jcontact.h"
#include "../../account/roster/jcontactresource.h"
#include "../../account/jaccount.h"
#include <jreen/jinglesession.h>
#include <qutim/servicemanager.h>
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include <QAudioInput>
#include <QAudioOutput>
#include <QDebug>

using namespace qutim_sdk_0_3;

namespace Jabber
{

class JingleButton;

typedef QMap<Account*, JingleSupport*> JingleMap;
Q_GLOBAL_STATIC(JingleMap, jingleMap)
Q_GLOBAL_STATIC(JingleGlobalSupport, globalSupport)


JingleGlobalSupport::JingleGlobalSupport()
{
}

JingleGlobalSupport::~JingleGlobalSupport()
{
}

void JingleGlobalSupport::onCallAction(QAction *action, QObject *object)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(object);
	Q_ASSERT(unit);
	if (JingleSupport *support = jingleMap()->value(unit->account()))
		support->onCallAction(action, object);
}

class JingleButton : public ActionGenerator
{
public:
	JingleButton()
	    : ActionGenerator(Icon("voicecall"), QT_TRANSLATE_NOOP("Jabber", "Voice call"),
	          globalSupport(), SLOT(onCallAction(QAction*,QObject*)))
	{
		setType(ActionTypeChatButton | ActionTypeContactList);
	}
	
	virtual void createImpl(QAction *action, QObject *obj) const
	{
		action->setCheckable(true);
		Q_UNUSED(obj);
//		JContact *contact = qobject_cast<JContact*>(obj);
//		JAccount *account = static_cast<JAccount*>(contact->account());
//		Jreen::JingleManager *manager = account->client()->jingleManager();
//		Q_ASSERT(contact);
//		bool hasFeature = false;
//		bool hasCall = false;
//		foreach (JContactResource *resource, contact->resources()) {
//			hasFeature |= manager->checkSupport(resource->features());
//			hasCall |= manager->hasSession(resource->id());
//		}
//		if (hasCall || hasFeature) {
//			action->setEnabled(true);
//			action->setCheckable(true);
//			action->setChecked(hasCall);
//		} else {
//			action->setDisabled(true);
//		}
	}
	
	virtual void showImpl(QAction *action, QObject *obj)
	{
		bool hasFeature = false;
		bool hasCall = false;
		ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
		Q_ASSERT(unit);
		JAccount *account = static_cast<JAccount*>(unit->account());
		Jreen::JingleManager *manager = account->client()->jingleManager();
		if (JContactResource *resource = qobject_cast<JContactResource*>(obj)) {
			hasFeature = manager->checkSupport(resource->features());
			hasCall = manager->hasSession(resource->id());
		} else if (JContact *contact = qobject_cast<JContact*>(obj)) {
			foreach (JContactResource *resource, contact->resources()) {
				hasFeature |= manager->checkSupport(resource->features());
				hasCall |= manager->hasSession(resource->id());
			}
		}
		if (hasCall || hasFeature) {
			action->setEnabled(true);
			action->setChecked(hasCall);
		} else {
			action->setDisabled(true);
		}
	}

	virtual void hideImpl(QAction *action, QObject *obj)
	{
		Q_UNUSED(action);
		Q_ASSERT(obj);
	}
};

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

void init_button(JingleButton *button)
{
	MenuController::addAction<JContact>(button);
	MenuController::addAction<JContactResource>(button);
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(JingleButton, button, init_button(x.data()))

JingleSupport::JingleSupport() : m_client(0)
{
	qDebug() << Q_FUNC_INFO;
	Q_UNUSED(button());
//	m_button.reset(new JingleButton(this, SLOT(onCallAction(QAction*,QObject*))));
//	m_button.reset(new JingleButton(this));
//	if (ServicePointer<QObject> chatForm("ChatForm"))
//		onServiceChanged(chatForm.name(), chatForm.data());
}

JingleSupport::~JingleSupport()
{
	jingleMap()->remove(m_account);
}

void JingleSupport::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
{
	qDebug() << Q_FUNC_INFO;
	Q_UNUSED(account);
	m_account = static_cast<JAccount*>(account);
	m_client = params.item<Jreen::Client>();
	connect(m_client->jingleManager(), SIGNAL(sessionCreated(Jreen::JingleSession*)),
	        SLOT(onSessionCreated(Jreen::JingleSession*)));
	jingleMap()->insert(m_account, this);
}

//void JingleSupport::onServiceChanged(const QByteArray &name, QObject *object)
//{
//	if (object && name == "ChatForm") {
//		QMetaObject::invokeMethod(object, "addAction",
//		                          Q_ARG(ActionGenerator*, m_button.data()));
//	}
//}

void JingleSupport::onCallAction(QAction *action, QObject *object)
{
	JContactResource *resource = qobject_cast<JContactResource*>(object);
	Jreen::JingleManager *manager = m_client->jingleManager();
	if (!resource) {
		JContact *contact = qobject_cast<JContact*>(object);
		Q_ASSERT(contact);
		if (action->isChecked()) {
			foreach (resource, contact->resources()) {
				if (manager->checkSupport(resource->features()))
					break;
				else
					resource = 0;
			}
		} else {
			foreach (resource, contact->resources()) {
				if (Jreen::JingleSession *session = manager->session(resource->id()))
					session->terminate();
			}
			return;
		}
		if (!resource)
			return;
	}
	Jreen::JingleSession *session = manager->createSession(resource->id());
	session->addContent(QLatin1String("audio"), QLatin1String("voice"));
}

void JingleSupport::onSessionCreated(Jreen::JingleSession *session)
{
	qDebug() << Q_FUNC_INFO << session;
	if (session->isIncoming()) {
		ChatUnit *unit = unitBySession(session);
		if (!unit) {
			session->terminate();
			connect(session, SIGNAL(terminated()), SLOT(deleteLater()));
			return;
		}
		foreach (QAction *action, button()->actions(unit)) {
			action->setEnabled(true);
			action->setChecked(true);
		}
	}
	connect(session, SIGNAL(contentAdded(Jreen::JingleContent*)),
	        SLOT(onContentAdded(Jreen::JingleContent*)));
	connect(session, SIGNAL(terminated()), SLOT(onSessionTerminated()));
	foreach (const QString &name, session->contents())
		onContentAdded(session->content(name));
}

void JingleSupport::onSessionTerminated()
{
	Jreen::JingleSession *session = qobject_cast<Jreen::JingleSession*>(sender());
	Q_ASSERT(session);
	session->deleteLater();
	ChatUnit *unit = unitBySession(session, false);
	foreach (QAction *action, button()->actions(unit))
		action->setChecked(false);
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

ChatUnit *JingleSupport::unitBySession(Jreen::JingleSession *session, bool create)
{
	ChatUnit *unit = m_account->unit(session->jid(), create);
	JContactResource *resource = qobject_cast<JContactResource*>(unit);
	if (!resource)
		return 0;
	if (JContact *contact = qobject_cast<JContact*>(resource->upperUnit()))
		unit = contact;
	return unit;
}

}

//#endif
