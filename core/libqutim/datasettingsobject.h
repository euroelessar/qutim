#ifndef QUTIM_SDK_0_3_DATASETTINGSOBJECT_H
#define QUTIM_SDK_0_3_DATASETTINGSOBJECT_H

#include "menucontroller.h"
#include "settingslayer.h"
#include "dataforms.h"

namespace qutim_sdk_0_3 {

class DataSettingsItemPrivate;
class DataSettingsGenerator;
class DataSettingsObjectPrivate;

class LIBQUTIM_EXPORT DataSettingsObject : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DataSettingsObject)
    Q_PROPERTY(qutim_sdk_0_3::DataItem item READ item NOTIFY itemChanged)
    Q_PROPERTY(QObject *controller READ controller WRITE setController NOTIFY controllerChanged)
public:
    DataSettingsObject(QObject *parent = 0);
    ~DataSettingsObject();
    
    qutim_sdk_0_3::DataItem item() const;
    void setController(QObject *controller);
    QObject *controller() const;
    
public slots:
    void load();
    void save(const qutim_sdk_0_3::DataItem &item);
    void cancel();
    
protected:
    virtual void loadImpl() = 0;
    virtual void saveImpl(const qutim_sdk_0_3::DataItem &item) = 0;
    virtual void cancelImpl();
    virtual void setControllerImpl(QObject *controller);
    void setItem(const qutim_sdk_0_3::DataItem &item);
    
signals:
    void itemChanged(const qutim_sdk_0_3::DataItem &item);
    void controllerChanged(QObject *controller);
    
private:
    QScopedPointer<DataSettingsObjectPrivate> d_ptr;
};

class LIBQUTIM_EXPORT DataSettingsItem : public SettingsItem
{
	Q_DECLARE_PRIVATE(DataSettingsItem)
public:
	DataSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text);
	DataSettingsItem(Settings::Type type, const LocalizedString &text);
	virtual ~DataSettingsItem();

protected:
    virtual DataSettingsObject *createObject() = 0;
	virtual const ObjectGenerator *generator() const;
    friend class DataSettingsGenerator;
};

template<typename T>
class GeneralDataSettingsItem : public DataSettingsItem
{
public:
	GeneralDataSettingsItem(Settings::Type type, const QIcon &icon, const LocalizedString &text)
			: DataSettingsItem(type, icon, text) {}
	GeneralDataSettingsItem(Settings::Type type, const LocalizedString &text)
			: DataSettingsItem(type, text) {}
	virtual ~GeneralDataSettingsItem() {}
protected:
    virtual DataSettingsObject *createObject()
    {
        return new T();
    }
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_DATASETTINGSOBJECT_H
