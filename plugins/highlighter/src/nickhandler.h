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


#ifndef HIGHLIGHTER_MESSAGEHANDLER_H
#define HIGHLIGHTER_MESSAGEHANDLER_H
#include <qutim/messagehandler.h>
#include <QRegExp>
#include <QLatin1String>
#include <QStringRef>
#include <QTextDocument>
#include <QChar>

#include <qutim/conference.h>

namespace Highlighter {

class NickHandler : public QObject, public qutim_sdk_0_3::MessageHandler
{
    Q_OBJECT
public:
	explicit NickHandler();
protected:
    void doHandle(qutim_sdk_0_3::Message &message, const MessageHandler::Handler &handler) override;
public slots:
	void loadSettings();
private:
	bool m_enableAutoHighlights;
	QList<QRegExp> m_regexps;
};

} // namespace Highlighter

#endif // HIGHLIGHTER_MESSAGEHANDLER_H

