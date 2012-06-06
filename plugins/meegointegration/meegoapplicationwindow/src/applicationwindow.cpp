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

#include "applicationwindow.h"
#include <QDeclarativeContext>
#include <qutim/thememanager.h>
#include <QDeclarativeEngine>

#include "servicemanagerwrapper.h"
#include "passworddialogwrapper.h"
#include "authdialogwrapper.h"
#include "addcontactdialogwrapper.h"
#include "aboutdialogwrapper.h"
#include "joingroupchatwrapper.h"
#include "settingswrapper.h"
#include "quickwidgetproxy.h"
#include "quickregexpservice.h"
#include "quickconfig.h"
#include "notificationwrapper.h"
#include "thememanagerwrapper.h"
#include "../../../../core/libqutim/statisticshelper_p.h"

#include "menumodel.h"
#include "addaccountdialogwrapper.h"
#include <QApplication>
#include <QGLWidget>
#include <MDeclarativeCache>
#include <QDeclarativeContext>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

ApplicationWindow::ApplicationWindow()
{
	QApplication::setStyle(QLatin1String("Plastique"));
	m_view = MDeclarativeCache::qDeclarativeView();
	ServiceManagerWrapper::init();
	MenuModel::init();
	PasswordDialogWrapper::init();
	AuthDialogWrapper::init();
	AddContactDialogWrapper::init();
	AboutDialogWrapper::init();
	JoinGroupChatWrapper::init();
	SettingsWrapper::init();
	AddAccountDialogWrapper::init();
	NotificationWrapper::init();
	qmlRegisterType<QuickRegExpService>("org.qutim", 0, 3, "RegExpService");
	qmlRegisterType<QuickConfig>("org.qutim", 0, 3, "Config");
	qmlRegisterType<QuickWidgetProxy>("org.qutim", 0, 3, "WidgetProxy");
	qmlRegisterType<StatisticsHelper>("org.qutim", 0, 3, "Statistics");
	ThemeManagerWrapper::init();

	QFont font;
	font.setFamily(QLatin1String("Nokia Pure"));
	font.setPointSize(24);
	qApp->setFont(font);
	//setOptimizationFlags(QGraphicsView::DontSavePainterState);
	QApplication::setGraphicsSystem(QLatin1String("raster"));

	m_view->setViewport(new QGLWidget());
	// These seem to give the best performance
//	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//	viewport->setFocusPolicy(Qt::NoFocus);
//	setFocusPolicy(Qt::StrongFocus);
//	setAttribute(Qt::WA_NoSystemBackground);
//	setAttribute(Qt::WA_OpaquePaintEvent);
//	viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
//	viewport()->setAttribute(Qt::WA_NoSystemBackground);
	
	QString filePath = ThemeManager::path(QLatin1String("declarative"),
	                                      QLatin1String("meego"));
	m_view->rootContext()->setContextProperty(QLatin1String("application"), this);
	m_view->setSource(QUrl::fromLocalFile(filePath + QLatin1String("/Main.qml")));
	m_view->showFullScreen();

}

void ApplicationWindow::showWidget(QWidget *widget)
{
	if (widget)
	{
		connect(widget,SIGNAL(destroyed()),this,SLOT(closeWidget()));
		emit widgetShown(widget);
	}
}

QDeclarativeEngine *ApplicationWindow::engine() const
{
	return m_view->engine();
}

void ApplicationWindow::closeWidget()
{
	emit widgetClosed();
}
}

