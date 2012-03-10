/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
** Copyright © 2012 Vsevolod Velichko <torkvema@gmail.com>
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

#include <QStringBuilder>
#include "nickhandler.h"
#include <qutim/config.h>
#include <qutim/chatsession.h>

namespace Highlighter {

using namespace qutim_sdk_0_3;

NickHandler::NickHandler()
{
	loadSettings();
}

void NickHandler::loadSettings()
{
	Config cfg;
	cfg.beginGroup("highlighter");

	m_enableAutoHighlights = cfg.value("enableAutoHighlights", true);
	m_regexps.clear();
	int count = cfg.beginArray(QLatin1String("regexps"));
	for (int i = 0; i < count; i++) {
		cfg.setArrayIndex(i);
		QRegExp regExp = cfg.value(QLatin1String("regexp"), QRegExp());

		m_regexps << regExp;
	}
	cfg.endGroup();
}

NickHandler::Result NickHandler::doHandle(Message &message, QString *)
{

	if(!message.isIncoming())
		return NickHandler::Accept;
	Conference *conference = qobject_cast<Conference*>(message.chatUnit());
	if (!conference)
		return NickHandler::Accept;
	Buddy *me = conference->me();
	if (!me)
		return NickHandler::Accept;
	const QString myNick = me->name();

	if(m_enableAutoHighlights)
	{
		QLatin1Literal first = "\\b";
		QLatin1Literal last = "\\b";

		if (!myNick.at(0).isLetterOrNumber())
			first = "\\B";

		if (!myNick.at(myNick.size() - 1).isLetterOrNumber())
			last = "\\B";

		QString autoPattern = first % QRegExp::escape(myNick) % last;

		QRegExp nickRegexp(autoPattern);
		nickRegexp.setCaseSensitivity(Qt::CaseInsensitive);
		nickRegexp.setPatternSyntax(QRegExp::RegExp);

		if(message.text().contains(nickRegexp))
		{
			message.setProperty("mention", true);
			return NickHandler::Accept;
		}
	}

	if(m_regexps.size())
	{
		for (int i = 0; i < m_regexps.size(); ++i) {
			if(message.text().contains(m_regexps.at(i)))
			{
				message.setProperty("mention", true);
				return NickHandler::Accept;
			}
		}
	}

	return NickHandler::Accept;
}

} // namespace Highlighter
