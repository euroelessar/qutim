/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QRegExp>

#include "utils.h"

QString Utils::stripEmail(const QString& email)
{
    QString stripped;

    //Verify email by RFC 2822 spec with additionaly verified domain
    QRegExp emailRx("(\\b[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*\\@(?:mail.ru|bk.ru|inbox.ru|list.ru|corp.mail.ru)\\b)",
					Qt::CaseInsensitive);
    int matchIndex = emailRx.indexIn(email);

    if (matchIndex >= 0)
    {//email is compliant
        stripped = emailRx.cap(1);
    }
    return stripped;
}

QString Utils::toHostPortPair(const QHostAddress& host, quint32 port)
{
    return QString("%1:%2").arg(host.toString()).arg(port);
}

