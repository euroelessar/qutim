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

#ifndef NOTIFICATIONMANAGERWRAPPER_H
#define NOTIFICATIONMANAGERWRAPPER_H

#include <qutim/notification.h>
#include <qdeclarative.h>

namespace MeegoIntegration
{
class NotificationManagerWrapper : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManagerWrapper();
	
	static void init();
	
public slots:
	void setBackendState(const QString &type, bool enabled);
	void enableBackend(const QString &type);
	void disableBackend(const QString &type);
	bool isBackendEnabled(const QString &type);
	
private slots:
	void onBackendStateChanged(const QByteArray &type, bool enabled);

signals:
	void backendStateChanged(const QString &type, bool enabled);

};

class NotificationBackendWrapper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
public:
	NotificationBackendWrapper();
	virtual ~NotificationBackendWrapper();
	
	QString type() const;
	void setType(const QString &type);
	bool isEnabled() const;
	void setEnabled(bool enabled);
	
signals:
	void typeChanged(const QString &type);
	void enabledChanged(bool enabled);

private slots:
	void onBackendStateChanged(const QByteArray &type, bool enabled);
	
private:
	QByteArray m_type;
};
}

QML_DECLARE_TYPE(MeegoIntegration::NotificationManagerWrapper)

#endif // NOTIFICATIONMANAGERWRAPPER_H
