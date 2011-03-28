/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef KDEABOUTAPPDIALOG_H
#define KDEABOUTAPPDIALOG_H

#include <qutim/libqutim_global.h>
#include <kaboutapplicationdialog.h>
#include <QPointer>

class KdeAboutAppDialog : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "AboutDialog")
	Q_CLASSINFO("Uses", "ContactList")
public:
    KdeAboutAppDialog();
	~KdeAboutAppDialog();
public slots:
	void showWidget();
private:
	QPointer<KAboutApplicationDialog> m_widget;
	KAboutData *m_data;
};

#endif // KDEABOUTAPPDIALOG_H
