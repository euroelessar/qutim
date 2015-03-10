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

static bool isWord(QChar ch)
{
	return ch.isLetterOrNumber() || ch.isMark() || ch == QLatin1Char('_');
}

MessageHandlerAsyncResult NickHandler::doHandle(Message &message)
{
	if(!message.isIncoming())
		return makeAsyncResult(Accept, QString());

	Conference *conference = qobject_cast<Conference*>(message.chatUnit());
	if (!conference)
		return makeAsyncResult(Accept, QString());

	Buddy *me = conference->me();
	if (!me)
		return makeAsyncResult(Accept, QString());

	const QString myNick = me->name();

	if (m_enableAutoHighlights) {
		const QString text = message.text();
		int pos = 0;
		while ((pos = text.indexOf(myNick, pos, Qt::CaseInsensitive)) != -1) {
			if ((pos == 0 || !isWord(text.at(pos - 1)))
					&& (pos + myNick.size() == text.size() || !isWord(text.at(pos + myNick.size())))) {
				message.setProperty("mention", true);
				return makeAsyncResult(Accept, QString());
			}
			++pos;
		}
	}

	if (m_regexps.size()) {
		for (int i = 0; i < m_regexps.size(); ++i) {
			if (message.text().contains(m_regexps.at(i))) {
				message.setProperty("mention", true);
				return makeAsyncResult(Accept, QString());
			}
		}
	}

	return makeAsyncResult(Accept, QString());
}

} // namespace Highlighter
