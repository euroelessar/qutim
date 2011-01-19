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
