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
		Q_OBJECT
		Q_DISABLE_COPY(MenuController)
		Q_DECLARE_PRIVATE(MenuController)
	public:
		MenuController(QObject *parent = 0);
		MenuController(MenuControllerPrivate &p, QObject *parent = 0);
		virtual ~MenuController();

		QMenu *menu(bool deleteOnClose = true) const;
		void addAction(const ActionGenerator *gen, const QList<QByteArray> &menu = QList<QByteArray>());
		template <int N>
		void addAction(const ActionGenerator *gen, const char (&menu)[N]);
		static void addAction(const ActionGenerator *gen, const QMetaObject *meta,
							  const QList<QByteArray> &menu = QList<QByteArray>());
		template <typename T>
		static void addAction(const ActionGenerator *gen,
							  const QList<QByteArray> &menu = QList<QByteArray>());
		template <typename T, int N>
		static void addAction(const ActionGenerator *gen, const char (&menu)[N]);
		template <int N>
		static void addAction(const ActionGenerator *gen, const QMetaObject *meta, const char (&menu)[N]);
	public slots:
		void showMenu(const QPoint &pos);
	protected:
		void setMenuOwner(MenuController *controller);
		QScopedPointer<MenuControllerPrivate> d_ptr;
	};

	template <int N>
	Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen, const char (&menu)[N])
	{
		addAction(gen, QByteArray::fromRawData(menu, N - 1).split('\0'));
	}

	template <typename T>
	Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen, const QList<QByteArray> &menu)
	{
		addAction(gen, &T::staticMetaObject, menu);
	}

	template <typename T, int N>
	Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen, const char (&menu)[N])
	{
		addAction(gen, &T::staticMetaObject, QByteArray::fromRawData(menu, N - 1).split('\0'));
	}

	template <int N>
	Q_INLINE_TEMPLATE void MenuController::addAction(const ActionGenerator *gen,
													 const QMetaObject *meta,
													 const char (&menu)[N])
	{
		addAction(gen, meta, QByteArray::fromRawData(menu, N - 1).split('\0'));
	}
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::MenuController*)

#endif // MENUUNIT_H
