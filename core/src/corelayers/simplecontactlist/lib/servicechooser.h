#ifndef QUTIM_SDK_0_3_SERVICECHOOSER_H
#define QUTIM_SDK_0_3_SERVICECHOOSER_H
#include "simplecontactlist_global.h"
#include <qutim/settingswidget.h>
#include <qutim/extensioninfo.h>
#include <QGroupBox>
#include <QRadioButton>
#include <QHash>

namespace Core {

class SIMPLECONTACTLIST_EXPORT ContactListSettingsExtention : public qutim_sdk_0_3::SettingsWidget
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
                   const qutim_sdk_0_3::LocalizedString &desc,
                   const QByteArray &currentService,
                   qutim_sdk_0_3::ExtensionInfoList &services,
                   QWidget *parent = 0);
    void setCurrentService(const QByteArray &service);
    QByteArray currentService() { return m_currentService; }
    qutim_sdk_0_3::ExtensionInfo currentServiceInfo();
    QByteArray service() { return m_service; }
signals:
    void serviceChanged(const QByteArray &newService, const QByteArray &oldService);
private slots:
    void onButtonToggled(bool checked);
    void onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject);
private:
    QLayout *m_layout;
    QHash<QByteArray, QRadioButton*> m_buttons;
    QHash<QByteArray, qutim_sdk_0_3::ExtensionInfo> m_infos;
    QByteArray m_service;
    QByteArray m_currentService;
};

} // namespace Core

Q_DECLARE_INTERFACE(Core::ContactListSettingsExtention, "org.qutim.core.ContactListSettingsExtention")

#endif // QUTIM_SDK_0_3_SERVICECHOOSER_H
