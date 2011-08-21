/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "applicationwindow.h"
#include <QDeclarativeContext>
#include <qutim/thememanager.h>
#include <QDeclarativeEngine>
#include "servicemanagerwrapper.h"

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

ApplicationWindow::ApplicationWindow()
{
	ServiceManagerWrapper::init();
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	engine()->addImportPath("/home/elessar/QtSDK/Madde/sysroots/harmattan-nokia-meego-arm-sysroot-1122-slim/usr/lib/qt4/imports");

	// These seem to give the best performance
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

	viewport()->setFocusPolicy(Qt::NoFocus);
	setFocusPolicy(Qt::StrongFocus);
	QString filePath = ThemeManager::path(QLatin1String("declarative"),
	                                      QLatin1String("meego/applicationwindow"));
	setSource(QUrl::fromLocalFile(filePath + QLatin1String("/Main.qml")));
}
}
