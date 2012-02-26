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

#include "antibossplugin.h"
#include <QSystemTrayIcon>
#include <QApplication>
#include <qutim/servicemanager.h>

AntiBossPlugin::AntiBossPlugin()
{
	m_shortcut = 0;
	m_hidden = false;
}

void AntiBossPlugin::init()
{
	addAuthor(QLatin1String("euroelessar"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Anti-boss plugin"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin makes your chatting invisible for boss"),
			PLUGIN_VERSION(0, 0, 1, 0),
			ExtensionIcon());
	setCapabilities(Loadable);
	GlobalShortcut::registerSequence("antiboss",
									 QT_TRANSLATE_NOOP("AntiBoss", "Show/hide all windows"),
									 QT_TRANSLATE_NOOP("AntiBoss", "Antiboss"),
									 QKeySequence("Ctrl+F11"));
}

bool AntiBossPlugin::load()
{
	if (!m_shortcut) {
		m_shortcut = new GlobalShortcut("antiboss", this);
		connect(m_shortcut, SIGNAL(activated()), this, SLOT(showHide()));
	}
	return true;
}

bool AntiBossPlugin::unload()
{
	if (m_shortcut) {
		if (m_hidden)
			showHide();
		delete m_shortcut;
		m_shortcut = 0;
	}
	return true;
}

bool AntiBossPlugin::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == QEvent::ShowToParent) {
		QWidget *w = qobject_cast<QWidget*>(obj);
		if (w && w->isWindow() && w->windowType() != Qt::Desktop) {
			m_widgets.insert(w->windowState(), w);
			w->hide();
		}
	}
	return QObject::eventFilter(obj, ev);
}

void AntiBossPlugin::showHide()
{
	if (QObject *tray = ServiceManager::getByName("TrayIcon")) {
		if (QSystemTrayIcon *icon = tray->findChild<QSystemTrayIcon*>())
			icon->setVisible(m_hidden);
	}
	if (m_hidden) {
		qApp->removeEventFilter(this);
		QMultiHash<Qt::WindowStates, QPointer<QWidget> >::const_iterator it = m_widgets.constBegin();
		for (;it!=m_widgets.constEnd();it++) {
			Qt::WindowStates state = it.key();
			it.value()->show();
			it.value()->setWindowState(state);
		}
		m_widgets.clear();
	} else {
		QWidgetList widgets = QApplication::topLevelWidgets();
		foreach (QWidget *widget, widgets) {
			if (widget->isVisible()) {
				widget->hide();
				m_widgets.insert(widget->windowState(), widget);
			}
		}
		qApp->installEventFilter(this);
	}
	m_hidden = !m_hidden;
}

QUTIM_EXPORT_PLUGIN(AntiBossPlugin)

