/****************************************************************************
 *  ircchannel_p.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCCHANNEL_P_H
#define IRCCHANNEL_P_H

#include "ircchannel.h"

namespace qutim_sdk_0_3 {

namespace irc {

class IrcJoinLeftActionGenerator : public ActionGenerator
{
public:
	IrcJoinLeftActionGenerator(QObject *receiver, const char *member);
protected:
	virtual void showImpl(QAction *action, QObject *obj);
};

typedef QSharedPointer<IrcChannelParticipant> ParticipantPointer;

class IrcChannelPrivate
{
public:
	ParticipantPointer me;
	QString name;
	QHash<QString, ParticipantPointer> users;
	QString topic;
	bool isJoined;
	bool autojoin;
	QString lastPassword;
	QString bookmarkName;
	bool reconnect;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNEL_P_H
