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

#ifndef PACKAGEENTRYWIDGET_H
#define PACKAGEENTRYWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "packageentry.h"
#include "packagepreviewwidget.h"

class PackageEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PackageEntryWidget(const PackageEntry &entry);
	
	PackageEntry entry() const { return m_entry; }
	void updateData();
signals:
	void buttonClicked();
private:
	PackagePreviewWidget *m_previewLabel;
	QLabel *m_detailsLabel;
	QPushButton *m_installButton;
	PackageEntry m_entry;
};

#endif // PACKAGEENTRYWIDGET_H
