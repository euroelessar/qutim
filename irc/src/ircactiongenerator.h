/****************************************************************************
 *  ircactiongenerator.h
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

#ifndef IRCACTIONGENERATOR_H
#define IRCACTIONGENERATOR_H

#include "ircglobal.h"
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3 {
namespace irc {

class IrcActionGeneratorPrivate;
class IrcCommandAlias;
class IrcChannelParticipant;
class IrcChannel;

class IrcActionGenerator : public ActionGenerator
{
public:
	IrcActionGenerator(const QIcon &icon, const LocalizedString &text, IrcCommandAlias *command);
protected:
	virtual QObject *generateHelper() const;
private:
	QScopedPointer<IrcActionGeneratorPrivate> d;
};

} // namespace irc
} // namespace qutim_sdk_0_3

#endif // IRCACTIONGENERATOR_H
