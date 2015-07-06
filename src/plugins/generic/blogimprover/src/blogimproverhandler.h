/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef BLOGIMPROVERHANDLER_H
#define BLOGIMPROVERHANDLER_H
#include <qutim/messagehandler.h>
#include <QRegExp>
#include <QLatin1String>
#include <QStringRef>
#include <QTextDocument>
#include <QChar>
#include <QMetaMethod>

#include <qutim/conference.h>
#include <qutim/chatsession.h>
#include <qutim/config.h>

namespace BlogImprover {

class BlogImproverHandler : public QObject, public qutim_sdk_0_3::MessageHandler
{
	Q_OBJECT
public:
	explicit BlogImproverHandler();
	enum BlogType {
		Nothing = -1,
		PstoJabber,
		PstoOscar,
		Juick,
		JuBo,
		Bnw
	};

protected:
	qutim_sdk_0_3::MessageHandlerAsyncResult doHandle(qutim_sdk_0_3::Message &message) override;
private:
	class HtmlLinker
	{
	public:
		HtmlLinker(qutim_sdk_0_3::ChatSession *session);

		bool isValid() const;

		QString create(const QString &text, const QString &label) const;

	private:
		bool m_valid = false;
		qutim_sdk_0_3::ChatSession *m_session = nullptr;
		QMetaMethod m_appendTextUrl;
		QString m_template;
	};

	qutim_sdk_0_3::ConfigValue<bool> m_enablePstoIntegration;
	qutim_sdk_0_3::ConfigValue<bool> m_enableJuickIntegration;
	qutim_sdk_0_3::ConfigValue<bool> m_enableBnwIntegration;
	QRegExp m_pstoNick;
	QRegExp m_pstoPost;
	QRegExp m_pstoComment;
	QRegExp m_pstoTag;

	QRegExp m_juickNick;
	QRegExp m_juickPost;
	QRegExp m_juickComment;

	void handlePsto(qutim_sdk_0_3::Message &message, const HtmlLinker &linker);
	void handleJuick(qutim_sdk_0_3::Message &message, const HtmlLinker &linker);
	void handleBnw(qutim_sdk_0_3::Message &message, const HtmlLinker &linker);
};

} // namespace BlogImprover

#endif // BLOGIMPROVERHANDLER_H

