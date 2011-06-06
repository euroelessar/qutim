/****************************************************************************
 * unitydock.h
 *  Copyright © 2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#ifndef UNITYDOCK_GN5KWDY6
#define UNITYDOCK_GN5KWDY6

#include "dockbase.h"

class QMenu;

class UnityDock : public DockBase
{
	Q_OBJECT
	public:
		UnityDock(QObject *p = NULL);
		~UnityDock();

	public slots:
		virtual void setIcon(const QIcon &);
		virtual void setOverlayIcon(const QIcon &);
		virtual void setMenu(QMenu *menu);
		virtual void setProgress(int progress);

		virtual void setBadge(const QString &badge);
		virtual void setCount(int count);
		virtual void setAlert(bool on = true);
	private:
		template<typename T> void sendMessage(const char *name, const T& val);
};

#endif /* end of include guard: UNITYDOCK_GN5KWDY6 */

