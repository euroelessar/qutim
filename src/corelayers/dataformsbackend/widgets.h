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
	virtual void setData(const QVariant &data);
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
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
	virtual void setData(const QVariant &data);
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
};

class DateTimeEdit : public QDateTimeEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DateTimeEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
};

class DateEdit : public QDateEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DateEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
};

class TextEdit : public QTextEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	TextEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
};

class LineEdit : public QLineEdit, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	LineEdit(DefaultDataForm *dataForm, const DataItem &item,
			 const QString &textHint = QString(), QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void textChanged(const QString &text);
private:
	void updateCompleteState(const QString &text);
	bool m_complete;
	bool m_mandatory;
	bool m_emitChangedSignal;
};

class SpinBox : public QSpinBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	SpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
};

class DoubleSpinBox : public QDoubleSpinBox, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	DoubleSpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
};

class IconListWidget : public QListWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	IconListWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
private:
	QHash<quint64,QListWidgetItem *> m_items;
	int m_type;
};

class IconWidget : public QWidget, public AbstractDataWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AbstractDataWidget)
public:
	IconWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent = 0);
	virtual DataItem item() const;
	virtual void setData(const QVariant &data);
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
public slots:
	void setIcon();
	void removeIcon();
private:
	void onDataChanged();
	void updatePixmap();
	int m_type;
	QString m_path;
	QLabel *m_pixmapWidget;
	QPixmap m_default;
	QSize m_size;
	bool m_emitChangedSignal;
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
	virtual void setData(const QVariant &data);
	QVariant data() const;
signals:
        void changed(const QString &name, const QVariant &data, qutim_sdk_0_3::AbstractDataForm *dataForm);
private slots:
	void onChanged();
private:
	QStringList m_alt;
	DataItem m_item;
};

}

#endif // WIDGETS_H
