/****************************************************************************
 *  actiontoolbar.h
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

#ifndef ACTIONTOOLBAR_H
#define ACTIONTOOLBAR_H

#include "actiongenerator.h"
#include <QToolBar>

namespace qutim_sdk_0_3
{
	struct ActionToolBarPrivate;

	class LIBQUTIM_EXPORT ActionToolBar : public QToolBar
	{
		Q_OBJECT
	public:
		explicit ActionToolBar(const QString &title, QWidget *parent = 0);
		explicit ActionToolBar(QWidget *parent = 0);
		virtual ~ActionToolBar();

		using QToolBar::addAction;
		QAction *addAction(ActionGenerator *generator);

		// This is done for Stacked toolbars like one in tabbed
		// chat window. This method calls setData(data) to every action,
		// created by addAction(ActionGenerator *generator) method
		void setData(const QVariant &var);
		QVariant data() const;
	private:
		QScopedPointer<ActionToolBarPrivate> p;
	};
}

#endif // ACTIONTOOLBAR_H
