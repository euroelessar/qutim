/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef ACTIONTOOLBAR_H
#define ACTIONTOOLBAR_H

#include "actiongenerator.h"
#include <QToolBar>
#include <QMetaType>

namespace qutim_sdk_0_3
{
class ActionToolBarPrivate;

class LIBQUTIM_EXPORT ActionToolBar : public QToolBar
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ActionToolBar)
public:
	explicit ActionToolBar(const QString &title, QWidget *parent = 0);
	explicit ActionToolBar(QWidget *parent = 0);
	virtual ~ActionToolBar();

	using QToolBar::addAction;
	QAction *addAction(ActionGenerator *generator);
	using QToolBar::insertAction;
	QAction *insertAction(QAction *before, ActionGenerator *generator);
	using QToolBar::removeAction;
	void removeAction(const ActionGenerator *generator);

	// This is done for Stacked toolbars like one in tabbed
	// chat window. This method calls setData(data) to every action,
	// created by addAction(ActionGenerator *generator) method
	void setData(const QVariant &var);
	QVariant data() const;

	void setMoveHookEnabled(bool enabled = true);
	// Toolbar id for appearance settings, by default id is "common"
	// 		void setId(const QString& id);
	using QToolBar::setIconSize;
    void setIconSize(const QSize & iconSize);
	using QToolBar::setToolButtonStyle;
    void setToolButtonStyle(Qt::ToolButtonStyle toolButtonStyle);
	void clear();
protected:
	void changeEvent(QEvent *e);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *);
private:
	QScopedPointer<ActionToolBarPrivate> d_ptr;
	Q_PRIVATE_SLOT(d_func(), void _q_size_action_triggered(QAction*))
	Q_PRIVATE_SLOT(d_func(), void _q_style_action_triggered(QAction*))
};
}

#endif // ACTIONTOOLBAR_H

