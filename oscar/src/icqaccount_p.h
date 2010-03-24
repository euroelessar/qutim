/****************************************************************************
 *  icqaccount_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/


#ifndef ICQACCOUNT_P_H
#define ICQACCOUNT_P_H

#include "icqaccount.h"
#include "messages_p.h"
#include <QTimer>
#include <qutim/passworddialog.h>

namespace qutim_sdk_0_3 {

namespace oscar {


class PasswordValidator: public QValidator
{
	Q_OBJECT
public:
	explicit PasswordValidator(QObject *parent = 0);
	virtual State validate(QString &input, int &pos) const;
};


struct IcqAccountPrivate
{
	OscarConnection *conn;
	Feedbag *feedbag;
	MessagesHandler *messagesHandler;
	QString name;
	bool avatars;
	QHash<quint64, Cookie*> cookies;
	Capabilities caps;
	QHash<QString, Capability> typedCaps;
	OscarStatus lastStatus;
	QTimer reconnectTimer;
	QHash<QString, IcqContact *> contacts;
	QList<RosterPlugin*> rosterPlugins;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNT_P_H
