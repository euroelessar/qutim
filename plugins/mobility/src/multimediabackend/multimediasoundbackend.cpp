/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "multimediasoundbackend.h"
#include <QMimeDatabase>
#include <QMimeType>
#include <qutim/debug.h>

namespace Core
{
MultimediaSoundBackend::MultimediaSoundBackend()
    : m_cache(24)
{
    QMimeDatabase db;
    const QStringList mimeTypes = QSoundEffect::supportedMimeTypes();
    for (const QString &name : mimeTypes)
        m_formats << db.mimeTypeForName(name).suffixes();
    m_formats.removeDuplicates();
}

void MultimediaSoundBackend::playSound(const QString &filename)
{
    QSoundEffect *effect = m_cache.object(filename);
    if (!effect) {
        effect = new QSoundEffect;
        effect->setSource(QUrl::fromLocalFile(filename));
        m_cache.insert(filename, effect);
    }

    effect->play();
}

QStringList MultimediaSoundBackend::supportedFormats()
{
    return m_formats;
}
}

