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
//#ifdef JABBER_HAVE_MULTIMEDIA

#include "../../../sdk/jabber.h"
#include <jreen/client.h>
#include <jreen/jinglemanager.h>
#include <jreen/jingleaudiocontent.h>

namespace Jabber
{

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
	
	virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
	
private slots:
	void onSessionCreated(Jreen::JingleSession *session);
	void onContentAdded(Jreen::JingleContent *content);
	void onStateChanged(Jreen::JingleContent::State state);
	
private:
	Jreen::Client *m_client;
	QHash<Jreen::JingleContent *, JingleHelper *> m_helpers;
};

}

//#endif

#endif // JINGLESUPPORT_H
