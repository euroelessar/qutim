#ifndef EDITABLEINFOLAYOUT_H
#define EDITABLEINFOLAYOUT_H

#include "infolayout.h"
#include <QGroupBox>

namespace Core
{

class AbstractInfoGroup
{
public:
	virtual InfoItem item() = 0;
};

}

Q_DECLARE_INTERFACE(Core::AbstractInfoGroup, "org.qutim.core.AbstractInfoGroup");

namespace Core
{

class EditableInfoLayout;

class InfoListWidget : public QWidget, public AbstractInfoGroup
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractInfoGroup)
public:
	InfoListWidget(QWidget *parent = 0);
	InfoListWidget(const InfoItem &def, QWidget *parent = 0);
	~InfoListWidget();
	void addRow(QWidget *data, QWidget *title = 0);
	void addRow(const InfoItem &item);
	InfoItem item();
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
	InfoItem m_def;
	int m_max;
private:
	void setRow(const WidgetLine &line, int row);
	void init();
};

class InfoListGroup : public QGroupBox, public AbstractInfoGroup
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractInfoGroup)
public:
	InfoListGroup(const InfoItem &item, QWidget *parent = 0);
	InfoListWidget *infoWidget() { return m_widget; };
	InfoItem item();
private:
	InfoListWidget *m_widget;
};

class InfoGroup : public QGroupBox, public AbstractInfoGroup
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractInfoGroup)
public:
	InfoGroup(const InfoItem &item, QWidget *parent = 0);
	EditableInfoLayout *infoLayout() { return m_layout; }
	InfoItem item();
private:
	EditableInfoLayout *m_layout;
};

class StringListGroup : public InfoListWidget
{
	Q_OBJECT
public:
	StringListGroup(const InfoItem &item, QWidget *parent = 0);
	InfoItem item();
};

class EditableInfoLayout : public AbstractInfoLayout, public AbstractInfoGroup
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractInfoGroup)
public:
	EditableInfoLayout(QWidget *parent = 0);
	bool addItems(const QList<InfoItem> &items);
	bool addItem(const InfoItem &item);
	InfoItem item();
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

#endif // EDITABLEINFOLAYOUT_H
