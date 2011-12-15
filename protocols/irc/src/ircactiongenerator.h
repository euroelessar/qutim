/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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
	~IrcActionGenerator();
	void enableAutoDeleteOfCommand(bool del = true);
protected:
	virtual QObject *generateHelper() const;
private:
	QScopedPointer<IrcActionGeneratorPrivate> d;
};

} // namespace irc
} // namespace qutim_sdk_0_3

#endif // IRCACTIONGENERATOR_H

