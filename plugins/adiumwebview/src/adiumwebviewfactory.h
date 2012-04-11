/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef QUICKCHATVIEWFACTORY_H
#define QUICKCHATVIEWFACTORY_H

#include <QWidget>
#include <qutim/adiumchat/chatviewfactory.h>

namespace qutim_sdk_0_3
{
class SettingsItem;
}

namespace Adium {

class WebViewFactory : public QObject, public Core::AdiumChat::ChatViewFactory
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewFactory)
	Q_CLASSINFO("Service", "ChatViewFactory")
	Q_CLASSINFO("Uses", "ChatForm")
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
	WebViewFactory();
	virtual QWidget *createViewWidget();
	virtual QObject *createViewController();
    virtual ~WebViewFactory();
	
signals:
	void settingsSaved();
	
private:
	qutim_sdk_0_3::SettingsItem *m_appearanceSettings;
};

} // namespace Adium

#endif // QUICKCHATVIEWFACTORY_H

