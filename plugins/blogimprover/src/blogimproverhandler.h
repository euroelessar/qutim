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

#include <qutim/conference.h>

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
	virtual qutim_sdk_0_3::MessageHandler::Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);
public slots:
	void loadSettings();
private:
	bool m_enablePstoIntegration;
	bool m_enableJuickIntegration;
	bool m_enableBnwIntegration;
	QRegExp m_pstoNick;
	QRegExp m_pstoPost;
	QRegExp m_pstoComment;
	QRegExp m_pstoTag;

	QRegExp m_juickNick;
	QRegExp m_juickPost;
	QRegExp m_juickComment;

	QString m_simplestyle;
	QString m_nickTemplate;
	QString m_postTemplate;
	QString m_tagTemplate;
	void handlePsto(qutim_sdk_0_3::Message &message);
	void handleJuick(qutim_sdk_0_3::Message &message);
	void handleBnw(qutim_sdk_0_3::Message &message);
};

} // namespace BlogImprover

#endif // BLOGIMPROVERHANDLER_H

