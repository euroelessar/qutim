/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2015 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "histview.h"
#include <QPoint>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QDir>
#include <qutim/icon.h>
#include <qutim/protocol.h>
#include <qutim/accountmanager.h>
#include <qutim/chatunit.h>
#include <QScrollBar>
#include <QStringBuilder>
#include <QDebug>
#include <qutim/systemintegration.h>
#include <qutim/shortcut.h>

namespace Core
{

void init(HistView *history)
{
	ActionGenerator *gen = new ActionGenerator(Icon("view-history"),
										QT_TRANSLATE_NOOP("Chat", "View History"),
										history,
										SLOT(onHistoryActionTriggered(QObject*)));
	gen->setType(ActionTypeChatButton|ActionTypeContactList);
	gen->setPriority(512);
	MenuController::addAction<ChatUnit>(gen);
}

HistView::HistView()
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		init(this);
	}
}

void HistView::onHistoryActionTriggered(QObject* object)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(object);

	showHistory(unit);
}

void HistView::showHistory(const ChatUnit *unit)
{
	unit = unit->getHistoryUnit();
	if (m_historyWindow) {
		m_historyWindow.data()->setUnit(unit);
		m_historyWindow.data()->raise();
	} else {
		m_historyWindow = new HistoryWindow(unit);
		m_historyWindow.data()->show();
	}
}

}
