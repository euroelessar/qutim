/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "formulahandler.h"
#include <qutim/debug.h>
#include <QUrl>
#include <QTextDocument>

using namespace qutim_sdk_0_3;

FormulaHandler::FormulaHandler()
{
	m_regexp.setPatternSyntax(QRegExp::RegExp);
	m_regexp.setPattern(QLatin1String("\\$\\$(.*)\\$\\$"));
	m_regexp.setMinimal(true);
	Q_ASSERT(m_regexp.isValid());
}

MessageHandlerAsyncResult FormulaHandler::doHandle(Message &message)
{
	int index = 0;
	int lastIndex = 0;
	const QString html = message.html();
	QString newHtml;
	newHtml.reserve(html.size());
    while ((index = m_regexp.indexIn(html, index)) >= 0) {
		html.midRef(lastIndex, index - lastIndex).appendTo(&newHtml);
		// html is already escaped
		const QString equation = m_regexp.cap(0);
		const QString url = QLatin1String("http://latex.codecogs.com/png.latex?")
		                    + QUrl::toPercentEncoding(unescape(m_regexp.cap(1)));
		newHtml += QLatin1String("<img src=\"");
		newHtml += url;
		newHtml += QLatin1String("\"alt=\"");
		newHtml += equation;
		newHtml += QLatin1String("\" title=\"");
		newHtml += equation;
		newHtml += QLatin1String("\">");
		index += m_regexp.cap(0).length();
		lastIndex = index;
    }
	html.midRef(lastIndex, html.size() - lastIndex).appendTo(&newHtml);
	message.setHtml(newHtml);
	return makeAsyncResult(Accept, QString());
}
