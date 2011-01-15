/****************************************************************************
 *  ircactiongenerator_p.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCACTIONGENERATOR_P_H
#define IRCACTIONGENERATOR_P_H

#include "ircactiongenerator.h"

namespace qutim_sdk_0_3 {
namespace irc {

class IrcActionGeneratorPrivate
{
public:
	IrcCommandAlias *command;
	bool commandAutoDelete;
};

class IrcActionsManager : public QObject
{
	Q_OBJECT
public:
	IrcActionsManager();
	static IrcActionsManager *instance();
public slots:
	void onIrcActionTriggered(QAction *action, QObject *controller);
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCACTIONGENERATOR_P_H
