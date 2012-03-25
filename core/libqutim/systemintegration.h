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

#ifndef SYSTEMINTEGRATION_H
#define SYSTEMINTEGRATION_H

#include "libqutim_global.h"
#include <QVariant>

class QAbstractSocket;

namespace qutim_sdk_0_3
{
class LIBQUTIM_EXPORT SystemIntegration : public QObject
{
	Q_OBJECT
public:
	enum Attribute {
		UserLogin,
		UserName,
		CurrentLanguage, // not the same as locale
		ExtensionsForMimeType, // /etc/mime.types
		TimeZone,
		TimeZoneName,
		TimeZoneShortName,
		IconSize, //default icon size
		CurrentProxyInfo
	};
	
	enum Operation
	{
		ShowWidget,
		KeepAliveSocket
	};

	enum IconSizeEnum
	{
		//By size
		IconSizeSmall,
		IconSizeMedium,
		IconSizeLarge,
		IconSizeHuge,
		//By Role
		IconSizeToolBar,
		IconSizeContactsView,
		IconSizeListView,
		IconSizeIconView
	};

	enum Priority
	{
		BaseSystem = 0, // Unix
		WindowManager = 127, // X11
		DesktopEnvironment = 255 // KDE
	};

	SystemIntegration();
	virtual ~SystemIntegration();

	virtual void init() = 0;
	virtual bool isAvailable() const = 0;
	virtual int priority() = 0;
	
	static void show(QWidget *widget);
	static void keepAlive(QAbstractSocket *socket);
	static QVariant value(Attribute attr, const QVariant &data = QVariant());
	static QVariant process(Operation act, const QVariant &data = QVariant());
//	virtual void show(QWidget *widget);

protected:
	virtual QVariant doGetValue(Attribute attr, const QVariant &data) const = 0;
	virtual QVariant doProcess(Operation act, const QVariant &data) const = 0;
	virtual bool canHandle(Attribute attribute) const = 0;
	virtual bool canHandle(Operation operation) const = 0;
	virtual void virtual_hook(int, void *);
	friend struct IntegrationData;
};
}

#endif // SYSTEMINTEGRATION_H

