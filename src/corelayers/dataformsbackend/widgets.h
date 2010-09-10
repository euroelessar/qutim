#ifndef WIDGETS_H
#define WIDGETS_H

#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QGridLayout>
#include <QPushButton>
#include "modifiablewidget.h"
#include <qutim/dataforms.h>

namespace Core
{

using namespace qutim_sdk_0_3;

class DataLayout;

static LocalizedString notSpecifiedStr = QT_TRANSLATE_NOOP("DataForms", "Not specified");

class Label : public QLabel, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	Label(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
private:
	DataItem m_item;
};

class CheckBox : public QCheckBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	CheckBox(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class ComboBox : public QComboBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	ComboBox(const QString &value, const QStringList &alternatives,
			 const char *validatorProperty, const DataItem &item,
			 QWidget *parent = 0);
	virtual DataItem item() const;
};

class DateTimeEdit : public QDateTimeEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DateTimeEdit(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class DateEdit : public QDateEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DateEdit(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class TextEdit : public QTextEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	TextEdit(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class LineEdit : public QLineEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	LineEdit(const DataItem &item, const QString &textHint = QString(), QWidget *parent = 0);
	virtual DataItem item() const;
};

class SpinBox : public QSpinBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	SpinBox(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class DoubleSpinBox : public QDoubleSpinBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DoubleSpinBox(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class IconListWidget : public QListWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	IconListWidget(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class IconWidget : public QWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	IconWidget(const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
public slots:
	void setIcon();
	void removeIcon();
private:
	int m_type;
	QString m_path;
	QLabel *m_pixmapWidget;
	QPixmap m_default;
	QSize m_size;
};

class ModifiableGroup : public QGroupBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	ModifiableGroup(const DataItem &item, QWidget *parent = 0);
	DataItem item() const;
private:
	ModifiableWidget *m_widget;
};

class DataGroup : public QGroupBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::AbstractDataWidget)
public:
	DataGroup(const DataItem &item, bool editable, QWidget *parent = 0);
	DataItem item() const;
private:
	DataLayout *m_layout;
};

class StringListGroup : public ModifiableWidget
{
	Q_OBJECT
public:
	StringListGroup(const DataItem &item, QWidget *parent = 0);
	DataItem item() const;
};

}

#endif // WIDGETS_H
