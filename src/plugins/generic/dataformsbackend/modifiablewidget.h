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
#ifndef MODIFIABLEWIDGET_H
#define MODIFIABLEWIDGET_H

#include <qutim/dataforms.h>
#include "abstractdatawidget.h"
#include "dataformsbackend.h"
#include <QPointer>

class QGridLayout;
class QPushButton;

namespace Core {

using namespace qutim_sdk_0_3;

class ModifiableWidget : public QWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	ModifiableWidget(const DataItem &item, DefaultDataForm *dataForm, QWidget *parent = 0);
	virtual ~ModifiableWidget();
	void addRow(QWidget *data, QWidget *title = 0);
	void addRow(const DataItem &item);
	DataItem item() const;
	int maxItemsCount() { return m_max; }
	void setMaxItemsCount(int max) { m_max = max; }
	bool isExpandable();
	void clear();
signals:
	void rowAdded();
	void rowRemoved();
private slots:
	void onAddRow();
	void onRemoveRow();
protected:
	QGridLayout *m_layout;
	QPushButton *m_addButton;
	struct WidgetLine {
		WidgetLine(QWidget *del, QWidget *d, QWidget *t = 0) :
				deleteButton(del), title(t), data(d) {}
		QWidget *deleteButton;
		QWidget *title;
		QWidget *data;
	};
	typedef QList<WidgetLine> WidgetList;
	WidgetList m_widgets;
	DataItem m_def;
	int m_max;
private:
	void setRow(const WidgetLine &line, int row);
	Qt::Alignment labelAlignment() const;
	mutable Qt::Alignment m_labelAlignment;
	mutable QPointer<QStyle> m_style;
};

} // namespace Core

#endif // MODIFIABLEWIDGET_H

