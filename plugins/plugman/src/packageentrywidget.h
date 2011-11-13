/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
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

