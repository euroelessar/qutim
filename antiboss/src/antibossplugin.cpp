/****************************************************************************
 * antibossplugin.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "antibossplugin.h"
#include <QSystemTrayIcon>
#include <QApplication>

AntiBossPlugin::AntiBossPlugin()
{
	m_shortcut = 0;
	m_hidden = false;
}

void AntiBossPlugin::init()
{
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Developer"),
			  QLatin1String("euroelessar@gmail.com"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Anti-boss plugin"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin makes your chatting invisible for boss"),
			PLUGIN_VERSION(0, 0, 1, 0),
			ExtensionIcon());
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
			if (!m_widgets.contains(w))
				m_widgets.append(w);
			w->hide();
		}
	}
	return QObject::eventFilter(obj, ev);
}

void AntiBossPlugin::showHide()
{
	if (QObject *tray = getService("TrayIcon")) {
		if (QSystemTrayIcon *icon = tray->findChild<QSystemTrayIcon*>())
			icon->setVisible(m_hidden);
	}
	if (m_hidden) {
		foreach (QWidget *widget, m_widgets) {
			if (widget)
				widget->show();
		}
		m_widgets.clear();
		qApp->removeEventFilter(this);
	} else {
		QWidgetList widgets = QApplication::topLevelWidgets();
		foreach (QWidget *widget, widgets) {
			if (widget->isVisible()) {
				widget->hide();
				m_widgets.append(widget);
			}
		}
		qApp->installEventFilter(this);
	}
	m_hidden = !m_hidden;
}

QUTIM_EXPORT_PLUGIN(AntiBossPlugin)
