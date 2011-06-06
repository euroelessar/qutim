/****************************************************************************
 * dockbase.h
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

#ifndef DOCKBASE_1FEA1T7G
#define DOCKBASE_1FEA1T7G

#include <QIcon>
#include <QMenu>
#include <QString>

class DockBase : public QObject
{
	Q_OBJECT
	public:
		DockBase(QObject *p = NULL);
		~DockBase() = 0;
	public slots:
		virtual void setIcon(const QIcon &);
		virtual void setOverlayIcon(const QIcon &);
		virtual void setMenu(QMenu *);
		virtual void setProgress(int);

		virtual void setBadge(const QString &);
		virtual void setAlert(bool on);
};


#endif /* end of include guard: DOCKBASE_1FEA1T7G */
