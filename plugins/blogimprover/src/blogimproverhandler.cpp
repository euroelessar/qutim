/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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
#include "blogimproverhandler.h"
#include <qutim/config.h>

namespace BlogImprover {

using namespace qutim_sdk_0_3;

BlogImproverHandler::BlogImproverHandler()
{
	loadSettings();
}

void BlogImproverHandler::loadSettings()
{
	Config cfg;
	cfg.beginGroup("BlogImprover");
	m_enablePstoIntegration = cfg.value(QLatin1String("enablePointIntegration"), true);
	m_enableJuickIntegration = cfg.value(QLatin1String("enableJuickIntegration"), true);
	m_enableBnwIntegration = cfg.value(QLatin1String("enableBnwIntegration"), true);
	cfg.endGroup();

	m_pstoNick.setPattern("(@[a-zA-Z0-9-_@\\.]+)\\b");
	m_pstoNick.setCaseSensitivity(Qt::CaseInsensitive);

	m_juickNick.setPattern("(@[a-zA-Z0-9-_@\\.]+)\\b");
	m_juickNick.setCaseSensitivity(Qt::CaseInsensitive);
	m_juickPost.setPattern("#\\d+\\b(?!/)");
	m_juickComment.setPattern("#\\d{3,}/\\d+\\b");

	m_pstoPost.setPattern("(#[a-z]+)\\b(?!/)");
	m_pstoComment.setPattern("(#[a-z]{4,}/\\d+)\\b");

	//m_pstoTag.setPattern("[*] ([^*,<]+(, [^*,<]+)*)");
}

MessageHandlerAsyncResult BlogImproverHandler::doHandle(Message &message)
{
	//qutim_sdk_0_3::MessageHandler::traceHandlers();

	if (!message.isIncoming())
		return makeAsyncResult(Accept, QString());

	ChatSession *session = ChatLayer::get(message.chatUnit(), false);
	HtmlLinker linker(session);
	if (!linker.isValid())
		return makeAsyncResult(Accept, QString());

	static QLatin1Literal jids[] = {
		QLatin1Literal("p@point.im"),
		QLatin1Literal("6571781"),
		QLatin1Literal("juick@juick.com"),
		QLatin1Literal("jubo@nologin.ru"),
		QLatin1Literal("bnw@bnw.im")
	};

	const size_t count = sizeof(jids) / sizeof(jids[0]);
	const QString id = message.chatUnit()->id();
	BlogImproverHandler::BlogType blogtype(BlogImproverHandler::Nothing);
	for (size_t i = 0; i < count; ++i) {
		if ((id.size() == jids[i].size() && id == QLatin1String(jids[i].data()))
			|| (id.size() > jids[i].size() && id.startsWith(QLatin1String(jids[i].data())) && id[jids[i].size()] == '/')) {
			blogtype = BlogImproverHandler::BlogType(i);
			break;
		}
	}

	switch(blogtype) {
	case BlogImproverHandler::PstoJabber:
	case BlogImproverHandler::PstoOscar:
		handlePsto(message, linker);
		break;
	case BlogImproverHandler::Juick:
	case BlogImproverHandler::JuBo:
		handleJuick(message, linker);
		break;
	case BlogImproverHandler::Bnw:
		handleBnw(message, linker);
		break;
	default:
		break;
	}

	return makeAsyncResult(Accept, QString());
}

void BlogImproverHandler::handlePsto(Message &message, const HtmlLinker &linker)
{
	if(!m_enablePstoIntegration)
		return;
	QString html = message.html();
	int pos = 0;

	while ((pos = m_pstoPost.indexIn(html, pos)) != -1) {
		const QString id = m_pstoPost.cap(1);
		const QString toReplace = linker.create(id, id)
			% QStringLiteral(" (")
			% linker.create(QStringLiteral("S ") + id, QStringLiteral("S"))
			% QStringLiteral(" ")
			% linker.create(id + QStringLiteral("++"), QStringLiteral("++"))
			% QStringLiteral(" ")
			% linker.create(QStringLiteral("! ") + id, QStringLiteral("!"))
			% QStringLiteral(" ")
			% linker.create(QStringLiteral("~ ") + id, QStringLiteral("~"))
			% QStringLiteral(")");

		html.replace(pos, id.length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_pstoComment.indexIn(html, pos)) != -1) {
		const QString id = m_pstoComment.cap(1);
		const QString toReplace = linker.create(id, id)
			% QStringLiteral(" (")
			% linker.create(QStringLiteral("U ") + id, QStringLiteral("U"))
			% QStringLiteral(" ")
			% linker.create(QStringLiteral("! ") + id, QStringLiteral("!"))
			% QStringLiteral(" ")
			% linker.create(QStringLiteral("~ ") + id, QStringLiteral("~"))
			% QStringLiteral(" ")
			% linker.create(id + QStringLiteral("++"), QStringLiteral("++"))
			% QStringLiteral(")");

		html.replace(pos, id.length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_pstoNick.indexIn(html, pos)) != -1) {
		const QString id = m_pstoNick.cap(1);
		const QString toReplace = linker.create(id + QLatin1Char('+'), id);

		html.replace(pos, id.length(), toReplace);
		pos += toReplace.length();
	}

	message.setHtml(html);
}

void BlogImproverHandler::handleJuick(Message &message, const HtmlLinker &linker)
{
	if(!m_enableJuickIntegration)
		return;
	QString html = message.html();
	QString toReplace;

	int pos = 0;

	while ((pos = m_juickPost.indexIn(html, pos)) != -1) {
		const QString id = m_juickPost.cap(0);
		const QString toReplace = linker.create(id, id)
			% QStringLiteral(" (")
			% linker.create(QStringLiteral("S ") + id, QStringLiteral("S"))
			% QStringLiteral(" ")
			% linker.create(id + QStringLiteral("+"), QStringLiteral("+"))
			% QStringLiteral(" ")
			% linker.create(QStringLiteral("! ") + id, QStringLiteral("!"))
			% QStringLiteral(")");

		html.replace(pos, id.length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_juickComment.indexIn(html, pos)) != -1) {
		const QString id = m_juickComment.cap(0);
		const QString toReplace = linker.create(id, id)
			% QStringLiteral(" (")
			% linker.create(QStringLiteral("U ") + id, QStringLiteral("U"))
			% QStringLiteral(" ")
			% linker.create(QStringLiteral("! ") + id, QStringLiteral("!"))
			% QStringLiteral(" ")
			% linker.create(id + QStringLiteral("+"), QStringLiteral("+"))
			% QStringLiteral(")");

		html.replace(pos, id.length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_juickNick.indexIn(html, pos)) != -1) {
		const QString id = m_juickNick.cap(1);
		const QString toReplace = linker.create(id + QLatin1Char('+'), id);

		html.replace(pos, id.length(), toReplace);
		pos += toReplace.length();
	}

	message.setHtml(html);
}

void BlogImproverHandler::handleBnw(Message &message, const HtmlLinker &linker)
{
	Q_UNUSED(message);
	Q_UNUSED(linker);
	if(!m_enableBnwIntegration)
		return;
}

BlogImproverHandler::HtmlLinker::HtmlLinker(ChatSession *session)
{
	if (!session) {
		m_valid = false;
		return;
	}

	if (session->property("supportJavaScript").toBool()) {
		m_valid = true;
		m_template = QStringLiteral(
			"<span onclick=\"client.appendText('%1')\" " \
			"style=\"color:#007FFF; text-decoration: underline; cursor: pointer;\">" \
			"%2</span>"
		);
	} else {
		const QMetaObject *meta = session->metaObject();
		int index = meta->indexOfMethod("appendTextUrl(QString)");
		if (index < 0)
			return;
		m_valid = true;
		m_session = session;
		m_appendTextUrl = meta->method(index);
		m_template = QStringLiteral("<a href=\"%1\">%2</a>");
	}
}

bool BlogImproverHandler::HtmlLinker::isValid() const
{
	return m_valid;
}

QString BlogImproverHandler::HtmlLinker::create(const QString &text, const QString &label) const
{
	QString first;
	if (m_appendTextUrl.isValid()) {
		QUrl url;
		m_appendTextUrl.invoke(m_session, Q_RETURN_ARG(QUrl, url), Q_ARG(QString, text));
		first = QString::fromUtf8(url.toEncoded());
	} else {
		first = text;
	}
	return m_template.arg(first, label.toHtmlEscaped());
}

} // namespace BlogImprover
