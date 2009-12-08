/****************************************************************************
 *  menucontroller.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef MENUUNIT_H
#define MENUUNIT_H

#include "actiongenerator.h"
#include <QtGui/QMenu>

namespace qutim_sdk_0_3
{
	class MenuControllerPrivate;

	class LIBQUTIM_EXPORT MenuController : public QObject
	{
		Q_DISABLE_COPY(MenuController)
		Q_DECLARE_PRIVATE(MenuController)
	public:
		MenuController(QObject *parent = 0);
		MenuController(MenuControllerPrivate &p, QObject *parent = 0);
		virtual ~MenuController();

		QMenu *menu(bool deleteOnClose = true) const;
		void addAction(const ActionGenerator *gen);
		static void addAction(const ActionGenerator *gen, const QMetaObject *meta);
	protected:
		QScopedPointer<MenuControllerPrivate> d_ptr;
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::MenuController *)

#endif // MENUUNIT_H
