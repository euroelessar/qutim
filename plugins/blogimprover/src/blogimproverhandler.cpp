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
#include <qutim/chatsession.h>

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
	m_enablePstoIntegration = cfg.value(QLatin1String("enablePstoIntegration"), true);
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
	m_simplestyle = "color:#007FFF; text-decoration: underline; cursor: pointer;";

	m_postTemplate = "<span onclick=\"client.appendText('%1')\" style=\"%2\">%1</span>";
	m_nickTemplate = "<span onclick=\"client.appendText('%1+')\" style=\"%2\">%1</span>";
	m_tagTemplate = "<span onclick=\"client.appendText('S *%1')\" style=\"%2\">%1</span>";

}

void BlogImproverHandler::doHandle(qutim_sdk_0_3::Message &message, const qutim_sdk_0_3::MessageHandler::Handler &handler)
{
	ChatSession *session = ChatLayer::get(message.chatUnit(), false);
    if (!session || !session->property("supportJavaScript").toBool() || !message.isIncoming()) {
		handler(Accept, QString());
        return;
    }

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
		handlePsto(message);
		break;
	case BlogImproverHandler::Juick:
	case BlogImproverHandler::JuBo:
		handleJuick(message);
		break;
	case BlogImproverHandler::Bnw:
		handleBnw(message);
		break;
	default:
        break;
	}

    handler(Accept, QString());
}

void BlogImproverHandler::handlePsto(Message &message)
{
	if(!m_enablePstoIntegration)
		return;
	QString html = message.html();
	QString toReplace;
	static QRegExp removeLast("/*", Qt::CaseSensitive, QRegExp::Wildcard);
	int pos = 0;

	while ((pos = m_pstoPost.indexIn(html, pos)) != -1) {
		toReplace = m_postTemplate.arg(m_pstoPost.cap(1), m_simplestyle);

		toReplace += QLatin1Literal(" (")
				% QString("<span onclick=\"client.appendText('S %1')\" style=\"%2\">S</span> ")
				  .arg(m_pstoPost.cap(1), m_simplestyle)
				% QString("<span onclick=\"client.appendText('%1++')\" style=\"%2\">++</span> ")
				  .arg(m_pstoPost.cap(1), m_simplestyle)
				% QString("<span onclick=\"client.appendText('! %1')\" style=\"%2\">!</span> ")
				  .arg(m_pstoPost.cap(1), m_simplestyle)
				% QString("<span onclick=\"client.appendText('~ %1')\" style=\"%2\">~</span>")
				  .arg(m_pstoPost.cap(1), m_simplestyle)
				% QLatin1Literal(")");

		html.replace(pos, m_pstoPost.cap(1).length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_pstoComment.indexIn(html, pos)) != -1) {
		toReplace = m_postTemplate.arg(m_pstoComment.cap(1), m_simplestyle);

		toReplace += QLatin1Literal(" (")
				% QString("<span onclick=\"client.appendText('U %1')\" style=\"%2\">U</span> ")
				  .arg(QString(m_pstoComment.cap(1)).replace(removeLast, ""), m_simplestyle)
				% QString("<span onclick=\"client.appendText('! %1')\" style=\"%2\">!</span> ")
				  .arg(m_pstoComment.cap(1), m_simplestyle)
				% QString("<span onclick=\"client.appendText('~ %1')\" style=\"%2\">~</span> ")
				  .arg(m_pstoComment.cap(1), m_simplestyle)
				% QString("<span onclick=\"client.appendText('%1++')\" style=\"%2\">++</span>")
				  .arg(QString(m_pstoComment.cap(1)).replace(removeLast, ""), m_simplestyle)
				% QLatin1Literal(")");

		html.replace(pos, m_pstoComment.cap(1).length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_pstoNick.indexIn(html, pos)) != -1) {
		toReplace = m_nickTemplate.arg(m_pstoNick.cap(1), m_simplestyle);

		html.replace(pos, m_pstoNick.cap(1).length(), toReplace);
		pos += toReplace.length();
	}

	message.setHtml(html);
}

void BlogImproverHandler::handleJuick(Message &message)
{
	if(!m_enableJuickIntegration)
		return;
	QString html = message.html();
	QString toReplace;
	static QRegExp removeLast("/*", Qt::CaseSensitive, QRegExp::Wildcard);

	int pos = 0;

	while ((pos = m_juickPost.indexIn(html, pos)) != -1) {
		toReplace = m_postTemplate.arg(m_juickPost.cap(0), m_simplestyle);

		toReplace += QLatin1Literal(" (")
				% QString("<span onclick=\"client.appendText('S %1')\" style=\"%2\">S</span> ")
				  .arg(m_juickPost.cap(0), m_simplestyle)
				% QString("<span onclick=\"client.appendText('%1+')\" style=\"%2\">+</span> ")
				  .arg(m_juickPost.cap(0), m_simplestyle)
				% QString("<span onclick=\"client.appendText('! %1')\" style=\"%2\">!</span>")
				  .arg(m_juickPost.cap(0), m_simplestyle)
				% QLatin1Literal(")");

		html.replace(pos, m_juickPost.cap(0).length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_juickComment.indexIn(html, pos)) != -1) {
		toReplace = m_postTemplate.arg(m_juickComment.cap(0), m_simplestyle);

		toReplace += QLatin1Literal(" (")
				% QString("<span onclick=\"client.appendText('U %1')\" style=\"%2\">U</span> ")
				  .arg(QString(m_juickComment.cap(0)).replace(removeLast, ""), m_simplestyle)
				% QString("<span onclick=\"client.appendText('! %1')\" style=\"%2\">!</span> ")
				  .arg(m_juickComment.cap(0), m_simplestyle)
				% QString("<span onclick=\"client.appendText('%1+')\" style=\"%2\">+</span>")
				  .arg(QString(m_juickComment.cap(0)).replace(removeLast, ""), m_simplestyle)
				% QLatin1Literal(")");

		html.replace(pos, m_juickComment.cap(0).length(), toReplace);
		pos += toReplace.length();
	}

	pos = 0;

	while ((pos = m_juickNick.indexIn(html, pos)) != -1) {
		toReplace = m_nickTemplate.arg(m_juickNick.cap(1), m_simplestyle);

		html.replace(pos, m_juickNick.cap(1).length(), toReplace);
		pos += toReplace.length();
	}

	message.setHtml(html);
}

void BlogImproverHandler::handleBnw(Message &message)
{
	Q_UNUSED(message);
	if(!m_enableBnwIntegration)
		return;
}

} // namespace BlogImprover
