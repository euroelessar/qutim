#ifndef QUTIM_SDK_0_3_DATASETTINGSOBJECT_P_H
#define QUTIM_SDK_0_3_DATASETTINGSOBJECT_P_H

#include "datasettingsobject.h"
#include "settingslayer_p.h"
#include <QVBoxLayout>

namespace qutim_sdk_0_3 {

class DataSettingsObjectPrivate
{
public:
    qutim_sdk_0_3::DataItem item;
    QObject *controller;
};

class DataSettingsWidget : public SettingsWidget
{
	Q_OBJECT
public:
	DataSettingsWidget(DataSettingsObject *object);
	virtual ~DataSettingsWidget();

    void setController(QObject *controller);
    
protected slots:
    void onItemChanged(const qutim_sdk_0_3::DataItem &item);
    void onChanged();
    
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
    
private:
	QScopedPointer<DataSettingsObject> m_object;
    QVBoxLayout *m_layout;
    AbstractDataForm *m_form;
};

class DataSettingsItemPrivate : public SettingsItemPrivate
{
public:
};

class DataSettingsGenerator : public ObjectGenerator
{
public:
	DataSettingsGenerator(DataSettingsItem *item) : m_item(item) {}
	virtual ~DataSettingsGenerator() {}
    
protected:
	virtual QObject *generateHelper() const
	{
        return new DataSettingsWidget(m_item->createObject());
	}
	virtual const QMetaObject *metaObject() const
	{
		return &DataSettingsWidget::staticMetaObject;
	}
private:
	DataSettingsItem *m_item;
	mutable QPointer<QObject> m_object;
};

}

#endif // QUTIM_SDK_0_3_DATASETTINGSOBJECT_P_H
