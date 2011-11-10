/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "chatviewfactory.h"
#include <qutim/servicemanager.h>
#include <qutim/debug.h>

namespace Core
{

namespace AdiumChat
{

using namespace qutim_sdk_0_3;
ChatViewFactory *ChatViewFactory::instance()
{
	ChatViewFactory *f = ServiceManager::getByName<ChatViewFactory*>("ChatViewFactory");
	Q_ASSERT(f);
	return f;
}

UrlTokenList ChatViewFactory::parseUrls(const QString &text)
{
	UrlTokenList result;
	static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
	                          "([a-z]+(\\+[a-z]+)?://|www\\.)"
	                          "[\\w-]+(\\.[\\w-]+)*\\.\\w+"
	                          "(:\\d+)?"
	                          "(/[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@'&=~-]*"
	                          "(\\?[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*)?"
	                          "(#[\\w\\+\\.\\[\\]!%\\$/\\\\\\(\\)\\|,:;@&=~-]*)?)?",
	                          Qt::CaseInsensitive);
	Q_ASSERT(linkRegExp.isValid());
	int pos = 0;
	int lastPos = 0;
	while (((pos = linkRegExp.indexIn(text, pos)) != -1)) {
		UrlToken tok = { text.midRef(lastPos, pos - lastPos), QString() };
		if (!tok.text.isEmpty()) {
			if (!result.isEmpty() && result.last().url.isEmpty()) {
				QStringRef tmp = result.last().text;
				result.last().text = QStringRef(tmp.string(), tmp.position(), tmp.size() + tok.text.size());
			} else {
				result << tok;
			}
		}
		QString link = linkRegExp.cap(0);
		tok.text = text.midRef(pos, link.size());
		pos += link.size();
		if (link.startsWith(QLatin1String("www."), Qt::CaseInsensitive))
			link.prepend(QLatin1String("http://"));
		else if(!link.contains(QLatin1String("//")))
			link.prepend(QLatin1String("mailto:"));
		tok.url = link;
		result << tok;
		lastPos = pos;
	}
	if (!result.isEmpty() && result.last().url.isEmpty()) {
		result.last().text = text.midRef(result.last().text.position());
	} else {
		UrlToken tok = { text.midRef(lastPos), QString() };
		result << tok;
	}
	return result;
}
}

}

