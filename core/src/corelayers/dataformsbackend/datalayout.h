/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef DATALAYOUT_H
#define DATALAYOUT_H

#include "abstractdatawidget.h"
#include "dataformsbackend.h"
#include <QWidget>
#include <QGridLayout>
#include <QHostAddress>
#include <QPointer>
#include <qutim/dataforms.h>

class QLabel;
namespace Core
{

class DefaultDataForm;

class DataLayout : public QGridLayout, public AbstractDataWidget
{
	Q_INTERFACES(Core::AbstractDataWidget)
public:
    DataLayout(const qutim_sdk_0_3::DataItem &item, DefaultDataForm *dataForm, quint8 columns, QWidget *parent = 0);
	~DataLayout();
    qutim_sdk_0_3::DataItem item() const;
    qutim_sdk_0_3::DataItem item(bool hasSubitems) const;
    void addDataItem(const qutim_sdk_0_3::DataItem &item);
    void addDataItems(const QList<qutim_sdk_0_3::DataItem> &items);
	void addSpacer();
	void addRow(QWidget *widget) { addRow(0, widget); }
	void addRow(QWidget *title, QWidget *widget, Qt::Alignment widgetAligment = 0);
	bool isExpandable() { return m_expandable; }
	void setHorizontalSpacing(int spacing);
protected:
	Qt::Alignment labelAlignment();
private:
	mutable Qt::Alignment m_labelAlignment;
    mutable QPointer<QStyle> m_style;
	int m_row;
	struct WidgetLine {
		WidgetLine(QWidget *t, QWidget *d) :
			title(t), data(d)
		{}
		QWidget *title;
		QWidget *data;
	};
	QList<WidgetLine> m_widgets;
	bool m_expandable;
#ifndef QUTIM_MOBILE_UI
	quint8 m_columns;
	quint8 m_currentColumn;
#endif
};

}

Q_DECLARE_METATYPE(QHostAddress)

#endif // DATALAYOUT_H

