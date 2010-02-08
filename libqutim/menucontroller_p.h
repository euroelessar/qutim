/****************************************************************************
 *  menucontroller_p.h
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

#ifndef MENUCONTROLLER_P_H
#define MENUCONTROLLER_P_H

#include "menucontroller.h"

namespace qutim_sdk_0_3
{
	struct ActionInfo
	{
		ActionInfo(const ActionGenerator *g, const QList<QByteArray> &m) : gen(g), menu(m)
		{
			for (int i = 0; i < menu.size(); i++)
				hash << qHash(menu.at(i));
		}
		const ActionGenerator *gen;
		QList<QByteArray> menu;
		QList<uint> hash;
	};

	class MenuControllerPrivate
	{
		Q_DECLARE_PUBLIC(MenuController)
	public:
		MenuControllerPrivate(MenuController *c) : owner(0), q_ptr(c) {}
		QList<ActionInfo> actions;
		MenuController *owner;
		MenuController *q_ptr;
		QList<ActionInfo> allActions() const;
	};

	class DynamicMenu : public QMenu
	{
		Q_OBJECT
	public:
		DynamicMenu(const MenuControllerPrivate *d);
	private slots:
		void onAboutToShow();
		void onAboutToHide();
	private:
		const MenuControllerPrivate * const m_d;
	};
}

#endif // MENUCONTROLLER_P_H
