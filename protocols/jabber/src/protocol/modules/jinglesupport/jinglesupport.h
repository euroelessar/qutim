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

#ifndef JINGLESUPPORT_H
#define JINGLESUPPORT_H
#ifdef JABBER_HAVE_MULTIMEDIA

#include "../../../sdk/jabber.h"
#include <jreen/client.h>
#include <jreen/experimental/jinglemanager.h>
#include <jreen/experimental/jingleaudiocontent.h>
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3
{
class ChatUnit;
}

namespace Jabber
{

class JAccount;
class JingleButton;

class JingleGlobalSupport : public QObject
{
    Q_OBJECT
public:
    JingleGlobalSupport();
    ~JingleGlobalSupport();
	
public slots:
	void onCallAction(QAction *action, QObject *object);
};

class JingleHelper : public QObject
{
	Q_OBJECT
public:
	JingleHelper(Jreen::JingleAudioContent *content);
	
private:
};

class JingleSupport : public QObject, public JabberExtension
{
	Q_OBJECT
	Q_INTERFACES(Jabber::JabberExtension)
public:
    JingleSupport();
    ~JingleSupport();
	
	virtual void init(qutim_sdk_0_3::Account *account);
	
private slots:
	void onCallAction(QAction *action, QObject *object);
//	void onServiceChanged(const QByteArray &name, QObject *newObject);
	void onSessionCreated(Jreen::JingleSession *session);
	void onSessionTerminated();
	void onContentAdded(Jreen::JingleContent *content);
	void onStateChanged(Jreen::JingleContent::State state);
	
protected:
	qutim_sdk_0_3::ChatUnit *unitBySession(Jreen::JingleSession *session, bool create = true);
	
private:
//	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_button;
	Jreen::Client *m_client;
	JAccount *m_account;
	QHash<Jreen::JingleContent *, JingleHelper *> m_helpers;
	
	friend class JingleGlobalSupport;
};

}

#endif

#endif // JINGLESUPPORT_H
