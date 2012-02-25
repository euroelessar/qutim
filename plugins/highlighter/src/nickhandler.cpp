/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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


#include "nickhandler.h"
#include <qutim/debug.h>
#include <qutim/config.h>
#include <qutim/chatsession.h>
#include <QTextDocument>

namespace Highlighter {

using namespace qutim_sdk_0_3;

NickHandler::NickHandler() :
	QObject()
{
	loadSettings();
}

void NickHandler::loadSettings()
{
	Config cfg;
	cfg.beginGroup("highlighter");
	m_regexp = cfg.value("regexp", "");
	m_enableHighlights = cfg.value("enableHighlights", true);
	cfg.endGroup();
}

NickHandler::Result NickHandler::doHandle(Message &message, QString *)
{
//	ChatSession *session = ChatLayer::get(message.chatUnit(), false);
//    if (!session || !session->property("supportJavaScript").toBool())
//		return UrlHandler::Accept;

	Conference *conference = qobject_cast<Conference*>(message.chatUnit());
	if (!conference)
		return NickHandler::Accept;
	Buddy *me = conference->me();
	if (!me)
		return NickHandler::Accept;
	const QString myNick = me->name();

	QRegExp nickRegexp(myNick + "*");
	nickRegexp.setCaseSensitivity(Qt::CaseInsensitive);
	nickRegexp.setPatternSyntax(QRegExp::Wildcard);

	if(message.html().contains(nickRegexp))
		message.setProperty("mention", true);

	debug() << Q_FUNC_INFO;

	return NickHandler::Accept;
}


} // namespace Highlighter

