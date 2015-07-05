/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jconferenceconfig.h"
#include "ui_jconferenceconfig.h"
#include "../dataform/jdataform.h"
#include "jmucsession.h"
#include <QStringBuilder>

namespace Jabber
{
using namespace Jreen;
struct JConferenceConfigPrivate
{
	DataForm::Ptr form;
	MUCRoom *room;
};

JConferenceConfig::JConferenceConfig() : p(new JConferenceConfigPrivate)
{
	p->room = 0;
}

JConferenceConfig::~JConferenceConfig()
{
}

void JConferenceConfig::setControllerImpl(QObject *controller)
{
	JMUCSession *session = qobject_cast<JMUCSession*>(controller);
	if (!session)
		return;
	p->room = session->room();
	connect(p->room, SIGNAL(configurationReceived(Jreen::DataForm::Ptr)),
			this, SLOT(onConfigurationReceived(Jreen::DataForm::Ptr)));
}

void JConferenceConfig::loadImpl()
{
	p->room->requestRoomConfig();
}

void JConferenceConfig::saveImpl(const qutim_sdk_0_3::DataItem &item)
{
	JDataForm::convertFromDataItem(p->form, item);
	p->room->setRoomConfig(p->form);
}

void JConferenceConfig::cancelImpl()
{
}

void JConferenceConfig::onConfigurationReceived(const Jreen::DataForm::Ptr &form)
{
	p->form = form;
	setItem(JDataForm::convertToDataItem(form));
}

}

