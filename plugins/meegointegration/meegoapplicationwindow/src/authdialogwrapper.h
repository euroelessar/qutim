/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef AUTHDIALOGWRAPPER_H
#define AUTHDIALOGWRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quickauthdialog.h"
#include <qutim/contact.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
class AuthDialogWrapper : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QString messageText READ messageText WRITE setMessageText NOTIFY messageTextChanged)
	Q_PROPERTY(bool isIncoming READ isIncoming WRITE setIsIncoming NOTIFY isIncomingChanged)

public:
	AuthDialogWrapper();
	~AuthDialogWrapper();
	QString title() const;
	QString messageText() const;
	bool isIncoming();
	void setTitle(const QString &);
	void setMessageText(const QString &);
	void setIsIncoming(bool);
	void setContact(Contact *contact);
	Contact *contact();
	Q_INVOKABLE void accept();
	Q_INVOKABLE void cancel();
	static void init();
	static void showDialog(qutim_sdk_0_3::Contact* contact, const QString message, bool isIncoming, QuickAuthDialog * authDialog);

signals:
	void titleChanged();
	void messageTextChanged();
	void isIncomingChanged();
	void contactChanged();
	void shown();

private:
	QString m_dialogTitle;
	QString m_messageText;
	bool m_isIncoming;
	Contact *m_contact;
	static QuickAuthDialog *m_currentDialog;

};
}

#endif /* AUTHDIALOGWRAPPER_H */

