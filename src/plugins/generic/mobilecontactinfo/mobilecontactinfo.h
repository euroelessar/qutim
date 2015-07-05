/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef MOBILECONTACTINFO_H
#define MOBILECONTACTINFO_H

#include "QWidget"
#include <QScrollArea>
#include <qutim/buddy.h>
#include <qutim/inforequest.h>
#include <qutim/actionbox.h>
#include <qutim/status.h>

class QLabel;
class QGridLayout;
class QVBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

typedef InfoRequestFactory::SupportLevel SupportLevel;

class MobileContactInfoWindow : public QScrollArea
{
	Q_OBJECT
public:
	MobileContactInfoWindow(QWidget *parent = 0);
	void setObject(QObject *object, SupportLevel type);
private slots:
	void onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State state);
	void onRequestButton();
	void onSaveButton();
private:
	DataItem filterItems(const DataItem &item, bool readOnly = false);
	void filterItemsHelper(const DataItem &item, DataItem &result, bool readOnly);
	bool isItemEmpty(const DataItem &item);
private:
	InfoRequest *request;
	QObject *object;
	bool readWrite;
	QVBoxLayout *layout;
	QScrollArea *scrollArea;
	QScopedPointer<AbstractDataForm> dataWidget;
	ActionBox *actionBox;
	QAction *saveAction;
};

class MobileContactInfo : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ContactInfo")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "DataFormsBackend")
public:
	MobileContactInfo();
public slots:
	void show(QObject *object);
private:
	QPointer<MobileContactInfoWindow> info;
};

}

#endif // MOBILECONTACTINFO_H

