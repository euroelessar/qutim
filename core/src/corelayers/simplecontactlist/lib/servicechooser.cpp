#include "servicechooser.h"
#include <qutim/servicemanager.h>
#include <qutim/metaobjectbuilder.h>
#include <QApplication>
#include <QVBoxLayout>

namespace Core {

using namespace qutim_sdk_0_3;

ContactListSettingsExtention::ContactListSettingsExtention()
{
}

ContactListSettingsExtention::~ContactListSettingsExtention()
{
}

ServiceChooser::ServiceChooser(const QByteArray &service,
                               const LocalizedString &title,
                               const QByteArray &currentService,
                               ExtensionInfoList &services,
                               QWidget *parent) :
    QGroupBox(title, parent), m_layout(new QVBoxLayout(this)), m_service(service), m_currentService(currentService)
{

    foreach (const ExtensionInfo &service, services) {
        const QMetaObject *meta = service.generator()->metaObject();
        QByteArray name = meta->className();
        const char *desc = MetaObjectBuilder::info(meta, "SettingsDescription");
        if (!desc || !*desc)
            desc = name.constData();
        QRadioButton *button = new QRadioButton(QCoreApplication::translate("ContactList", desc), this);
        button->setObjectName(QString::fromLatin1(name));

        button->setChecked(name == m_currentService);
        connect(button, SIGNAL(toggled(bool)), SLOT(onButtonToggled(bool)));
        m_buttons.insert(name, button);
        m_infos.insert(name, service);
        m_layout->addWidget(button);
    }
    connect(ServiceManager::instance(),
            SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
            SLOT(onServiceChanged(QByteArray,QObject*,QObject*)));
}

void ServiceChooser::setCurrentService(const QByteArray &service)
{
    QRadioButton *button = m_buttons.value(service);
    if (!button)
        return;
    button->blockSignals(true);
    button->setChecked(true);
    emit serviceChanged(service, m_currentService);
    m_currentService = service;
    button->blockSignals(false);
}

ExtensionInfo ServiceChooser::currentServiceInfo()
{
    return m_infos.value(m_currentService);
}

void ServiceChooser::onButtonToggled(bool checked)
{
    if (!checked)
        return;
    QRadioButton *button = qobject_cast<QRadioButton*>(sender());
    Q_ASSERT(button && !button->objectName().isEmpty());
    QByteArray newService = button->objectName().toUtf8();
    Q_ASSERT(m_buttons.contains(newService));
    emit serviceChanged(newService, m_currentService);
    m_currentService = newService;
}

void ServiceChooser::onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject)
{
    if (name == m_service && oldObject->metaObject()->className() == m_currentService)
        setCurrentService(newObject->metaObject()->className());
}


} // namespace Core
