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
#ifndef JCONFERENCECONFIG_H
#define JCONFERENCECONFIG_H

#include <jreen/mucroom.h>
#include <qutim/icon.h>
#include <qutim/settingswidget.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JMUCSession;
struct JConferenceConfigPrivate;

class JConferenceConfig : public SettingsWidget
{
	Q_OBJECT
public:
	JConferenceConfig();
	~JConferenceConfig();
	
	virtual void setController(QObject *controller);
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
protected slots:
	void onConfigurationReceived(const Jreen::DataForm::Ptr &form);
	void onDataChanged();
private:
	QScopedPointer<JConferenceConfigPrivate> p;
};
}

#endif // JCONFERENCECONFIG_H

