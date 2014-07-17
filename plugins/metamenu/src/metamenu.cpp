/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2014 Nicolay Izoderov <nico-izo@ya.ru>
** Copyright © 2014 Ivan Vizir <define-true-false@yandex.com>
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

#include "metamenu.h"
#include <qutim/debug.h>
#include <qutim/chatsession.h>
#include <QMenuBar>
#include <QMainWindow>
#include <QTimer>

MetamenuPlugin::MetamenuPlugin ()
{
}

void MetamenuPlugin::init ()
{
	qutim_sdk_0_3::ExtensionIcon icon("info");
	addAuthor(QLatin1String("nicoizo"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "MetaMenu"),
			QT_TRANSLATE_NOOP("Plugin", "Ubuntu metamenu integration"),
			PLUGIN_VERSION(0, 0, 1, 1),
			icon
			);
	setCapabilities(Loadable);
}

bool MetamenuPlugin::load ()
{
	QTimer::singleShot(2000, this, SLOT(shot()));
	m_menu = new MetaMenuController(this);
	return true;
}

bool MetamenuPlugin::unload ()
{
	return true;
}

/*!
 * \brief MetamenuPlugin::oneOfChatWindows
 * \author viv
 * \return QMainWindow
 */
QWidget* MetamenuPlugin::oneOfChatWindows()
{
	QObject* obj = ServiceManager::getByName("ChatForm");
	QWidget *widget = 0;
	bool metZero = false;
	QWidgetList list;
	QMetaObject::invokeMethod(obj, "chatWidgets", Q_RETURN_ARG(QWidgetList, list));
	if(!list.size())
		return 0;
	foreach (QWidget *w, list)
		if (w) {
			widget = w->window();
			break;
		} else
			metZero = true;
	if (metZero) // TODO: this block should dissapear sometimes as well as variables used here
		qWarning() << "Zeros still appear in ChatForm's chatWidgets list.";
	return widget;
}

void MetamenuPlugin::shot() {
	QWidget* window = oneOfChatWindows();

	if(!m_menuBar && window)
		QMetaObject::invokeMethod(window, "getMenuBar", Q_RETURN_ARG(QMenuBar*, m_menuBar));

	QMenu* mainMenu = m_menu->menu(false);

	mainMenu->setTitle("&qutIM");

	if(m_menuBar && !m_added) {
		qDebug() << mainMenu;
		m_menuBar->addMenu(mainMenu);
		m_added = true;
	 }

	if(!m_added)
		QTimer::singleShot(1000, this, SLOT(shot()));
	else
		connect(window, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
}

void MetamenuPlugin::onDestroyed()
{
	m_added = false;
	m_menuBar = 0;
	delete m_menu;
	m_menu = new MetaMenuController(this);
	QTimer::singleShot(1000, this, SLOT(shot()));
}



QUTIM_EXPORT_PLUGIN(MetamenuPlugin)
