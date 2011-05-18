#ifndef CONTACTLISTSETTINGS_H
#define CONTACTLISTSETTINGS_H

#include "simplecontactlist_global.h"
#include <qutim/settingswidget.h>
#include <qutim/servicemanager.h>
#include <QRadioButton>]
#include <QGroupBox>
#include <QHash>
#include <QSet>

class QLayout;
class QVBoxLayout;

namespace qutim_sdk_0_3 {
	class ObjectGenerator;
}

namespace Core {

using namespace qutim_sdk_0_3;

class SIMPLECONTACTLIST_EXPORT ContactListSettingsExtention : public SettingsWidget
{
	Q_OBJECT
public:
	ContactListSettingsExtention();
	virtual ~ContactListSettingsExtention();
};

class ServiceChooser : public QGroupBox
{
	Q_OBJECT
public:
	ServiceChooser(const QByteArray &service,
				   const LocalizedString &desc,
				   const QByteArray &currentService,
				   ExtensionInfoList &services,
				   QWidget *parent = 0);
	void setCurrentService(const QByteArray &service);
	QByteArray currentService() { return m_currentService; }
	ExtensionInfo currentServiceInfo();
	QByteArray service() { return m_service; }
signals:
	void serviceChanged(const QByteArray &newService, const QByteArray &oldService);
private slots:
	void onButtonToggled(bool checked);
	void onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject);
private:
	QLayout *m_layout;
	QHash<QByteArray, QRadioButton*> m_buttons;
	QHash<QByteArray, ExtensionInfo> m_infos;
	QByteArray m_service;
	QByteArray m_currentService;
};

class ContactListSettings : public SettingsWidget
{
	Q_OBJECT
public:
	ContactListSettings();
	virtual ~ContactListSettings();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
private:
	void addService(const QByteArray &service, const LocalizedString &title);
	void addExtensionWidget(const QByteArray &service, const ObjectGenerator *gen, bool load);
private slots:
	void onServiceChanged(const QByteArray &newService, const QByteArray &oldService);
	void onModifiedChanged(bool haveChanges);
private:
	QVBoxLayout *m_layout;
	QHash<const ObjectGenerator*, SettingsWidget*> m_widgetCache;
	QHash<QByteArray, ServiceChooser*> m_serviceChoosers;
	QHash<QByteArray, const ObjectGenerator*> m_extensions;
	QHash<QByteArray, SettingsWidget*> m_extensionWidgets;
	QList<SettingsWidget*> m_staticExtensionWidgets;
	QSet<QByteArray> m_services;
	bool m_modified;
};

} // namespace Core

Q_DECLARE_INTERFACE(Core::ContactListSettingsExtention, "org.qutim.core.ContactListSettingsExtention")


#endif // CONTACTLISTSETTINGS_H
