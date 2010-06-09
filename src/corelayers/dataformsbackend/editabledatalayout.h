#ifndef EDITABLEDATALAYOUT_H
#define EDITABLEDATALAYOUT_H

#include "abstractdatalayout.h"
#include <QGroupBox>

namespace Core
{

class EditableDataLayout;

class DataListWidget : public QWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DataListWidget(QWidget *parent = 0);
	DataListWidget(const DataItem &def, QWidget *parent = 0);
	virtual ~DataListWidget();
	void addRow(QWidget *data, QWidget *title = 0);
	void addRow(const DataItem &item);
	DataItem item() const;
	int maxItemsCount() { return m_max; }
	void setMaxItemsCount(int max) { m_max = max; }
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
	void init();
};

class DataListGroup : public QGroupBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DataListGroup(const DataItem &item, QWidget *parent = 0);
	DataListWidget *dataWidget() { return m_widget; };
	DataItem item() const;
private:
	DataListWidget *m_widget;
};

class DataGroup : public QGroupBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DataGroup(const DataItem &item, QWidget *parent = 0);
	EditableDataLayout *infoLayout() { return m_layout; }
	DataItem item() const;
private:
	EditableDataLayout *m_layout;
};

class StringListGroup : public DataListWidget
{
	Q_OBJECT
public:
	StringListGroup(const DataItem &item, QWidget *parent = 0);
	DataItem item() const;
};

class EditableDataLayout : public AbstractDataLayout, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	EditableDataLayout(QWidget *parent = 0);
	bool addItem(const DataItem &item);
	DataItem item() const;
	static QWidget *getWidget(const DataItem &item);
private:
	struct WidgetLine {
		WidgetLine(QWidget *t, QWidget *d) :
			title(t), data(d)
		{}
		QWidget *title;
		QWidget *data;
	};
	QList<WidgetLine> m_widgets;
};

}

#endif // EDITABLEDATALAYOUT_H
