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

class Label : public QLabel, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	Label(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
private:
	DataItem m_item;
};

class CheckBox : public QCheckBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	CheckBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class ComboBox : public QComboBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	ComboBox(DefaultDataForm *dataForm,
			 const QString &value, const QStringList &alternatives,
			 const char *validatorProperty, const DataItem &item,
			 QWidget *parent = 0);
	virtual DataItem item() const;
};

class DateTimeEdit : public QDateTimeEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DateTimeEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class DateEdit : public QDateEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DateEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class TextEdit : public QTextEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	TextEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class LineEdit : public QLineEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	LineEdit(DefaultDataForm *dataForm, const DataItem &item,
			 const QString &textHint = QString(), QWidget *parent = 0);
	virtual DataItem item() const;
private slots:
	void textChanged(const QString &text);
private:
	void updateCompleteState(const QString &text);
	DefaultDataForm *m_dataForm;
	bool m_complete;
	bool m_mandatory;
};

class SpinBox : public QSpinBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	SpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class DoubleSpinBox : public QDoubleSpinBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DoubleSpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class IconListWidget : public QListWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	IconListWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
};

class IconWidget : public QWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	IconWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
public slots:
	void setIcon();
	void removeIcon();
private:
	DefaultDataForm *m_dataForm;
	int m_type;
	QString m_path;
	QLabel *m_pixmapWidget;
	QPixmap m_default;
	QSize m_size;
};

class ModifiableGroup : public QGroupBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	ModifiableGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	DataItem item() const;
private:
	ModifiableWidget *m_widget;
};

class DataGroup : public QGroupBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DataGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	DataItem item() const;
private:
	DataLayout *m_layout;
};

class StringListGroup : public ModifiableWidget
{
	Q_OBJECT
public:
	StringListGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	DataItem item() const;
};

}

#endif // WIDGETS_H
