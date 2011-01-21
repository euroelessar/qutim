/****************************************************************************
 *  actiontoolbar.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef ACTIONTOOLBAR_H
#define ACTIONTOOLBAR_H

#include "actiongenerator.h"
#include <QToolBar>

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
		QAction *insertAction(QAction *before,ActionGenerator *generator);

		// This is done for Stacked toolbars like one in tabbed
		// chat window. This method calls setData(data) to every action,
		// created by addAction(ActionGenerator *generator) method
		void setData(const QVariant &var);
		QVariant data() const;

		void setMoveHookEnabled(bool enabled = true);
		// Toolbar id for appearance settings, by default id is "common"
// 		void setId(const QString& id);
	protected:
		void changeEvent(QEvent *e);
		void mouseMoveEvent(QMouseEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void contextMenuEvent(QContextMenuEvent* event);
	private:
		QScopedPointer<ActionToolBarPrivate> d_ptr;
	};
}

#endif // ACTIONTOOLBAR_H
