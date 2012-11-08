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

#include "crypter.h"
#include <QCoreApplication>
#include <QtCrypto>

namespace Control {

Crypter::Crypter(const QByteArray &key) : m_key(new QCA::SymmetricKey(key))
{
    QCA::init();
    QCA::setAppName(QCoreApplication::applicationName());
}

Crypter::~Crypter()
{
}

QByteArray Crypter::encode(const QByteArray &data, bool *ok) const
{
    QCA::InitializationVector iv(16);
    QCA::Cipher cipher(QString("aes256"),
                       QCA::Cipher::CBC,
                       QCA::Cipher::DefaultPadding,
                       QCA::Encode,
                       *m_key.data(),
                       iv);
    QCA::SecureArray result = cipher.update(data);
    QCA::SecureArray final = cipher.final();
    if (!cipher.ok()) {
        *ok = false;
        qCritical("Something wrong");
        return QByteArray();
    }
    *ok = true;
    return iv.toByteArray()
            .append(result.toByteArray())
            .append(final.toByteArray())
            .toBase64();
}

} // namespace Control
