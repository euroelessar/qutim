/****************************************************************************
 *  tooltip.h
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

#ifndef TOOLTIPMANAGER_H
#define TOOLTIPMANAGER_H

#include "libqutim_global.h"
#include <QPoint>

namespace qutim_sdk_0_3
{
	class Contact;

	class LIBQUTIM_EXPORT ToolTip : public QObject
	{
		Q_OBJECT
	public:
		static ToolTip *instance();
		virtual void showText(const QPoint &pos, QObject *obj, QWidget *w = 0);
		inline void hideText() { showText(QPoint(), 0); }
	protected:
		ToolTip(QObject *parent = 0);
		QString html(Contact *contact, bool extra);
		bool eventFilter(QObject *, QEvent *);
	};
}

#endif // TOOLTIPMANAGER_H
