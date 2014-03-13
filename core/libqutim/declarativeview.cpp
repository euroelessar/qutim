/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "declarativeview.h"
#include "icon.h"
#include <QPointer>
#include <QLayout>
#include <QQmlEngine>
#include <QQuickImageProvider>
#include <qqml.h>
#include "systeminfo.h"

namespace qutim_sdk_0_3 {

DeclarativeView::DeclarativeView(QWindow *parent) :
	QQuickView(globalEngine(), parent)
{
}

DeclarativeView::~DeclarativeView()
{
}

static QPointer<QQmlEngine> staticGlobalEngine;

QQmlEngine *DeclarativeView::globalEngine()
{
    if (!staticGlobalEngine) {
		staticGlobalEngine = new QQmlEngine();
        staticGlobalEngine->addImportPath(SystemInfo::getPath(SystemInfo::SystemShareDir) + QStringLiteral("/imports"));
    }

	return staticGlobalEngine;
}

} // namespace qutim_sdk_0_3

#include "moc_declarativeview.cpp"
