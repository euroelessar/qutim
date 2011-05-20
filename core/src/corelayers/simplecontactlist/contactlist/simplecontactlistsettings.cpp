#include "simplecontactlistsettings.h"
#include <qutim/metaobjectbuilder.h>
#include <qutim/notification.h>
#include <qutim/objectgenerator.h>
#include <QVBoxLayout>
#include <QSet>

namespace Core {

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
		QByteArray desc = MetaObjectBuilder::info(meta, "SettingsDescription");
		if (desc.isEmpty())
			desc = name;
		QRadioButton *button = new QRadioButton(LocalizedString(desc).toString(), this);
		button->setObjectName(name);

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
	QByteArray newService = button->objectName().toAscii();
	Q_ASSERT(m_buttons.contains(newService));
	emit serviceChanged(newService, m_currentService);
	m_currentService = newService;
}

void ServiceChooser::onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject)
{
	if (name == m_service && oldObject->metaObject()->className() == m_currentService)
		setCurrentService(newObject->metaObject()->className());
}

ContactListSettings::ContactListSettings()
{
	m_layout = new QVBoxLayout(this);
	addService("ContactModel", tr("Model"));
	addService("ContactListWidget", tr("Widget style"));
	addService("ContactDelegate", tr("Contacts style"));

	m_layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<ContactListSettingsExtention>()) {
		QByteArray service = MetaObjectBuilder::info(gen->metaObject(), "ServiceSettings");
		if (service.isEmpty())
			addExtensionWidget("", gen, false);
		else
			m_extensions.insert(service, gen);
	}
}

ContactListSettings::~ContactListSettings()
{
}

void ContactListSettings::addService(const QByteArray &service, const LocalizedString &title)
{
	QObject *serviceObj = ServiceManager::getByName(service);
	if (!serviceObj)
		return;
	ExtensionInfoList services = ServiceManager::listImplementations(service);
	if (services.size() > 1) {
		QByteArray currentService = serviceObj->metaObject()->className();
		m_services.insert(currentService);
		ServiceChooser *chooser = new ServiceChooser(service, title, currentService, services, this);
		m_layout->addWidget(chooser);
		m_serviceChoosers.insert(service, chooser);
		connect(chooser,
				SIGNAL(serviceChanged(QByteArray,QByteArray)),
				SLOT(onServiceChanged(QByteArray,QByteArray)));
	}
}

void ContactListSettings::addExtensionWidget(const QByteArray &service, const ObjectGenerator *gen, bool load)
{
	SettingsWidget *widget = m_widgetCache.value(gen);
	if (!widget) {
		widget = gen->generate<ContactListSettingsExtention>();
		if (!widget)
			return;
		widget->setParent(this);
		if (!service.isEmpty())
			load = true;
	}
	if (!service.isEmpty())
		m_extensionWidgets.insert(service, widget);
	else
		m_staticExtensionWidgets.push_back(widget);
	if (load)
		widget->load();
	m_widgetCache.insert(gen, widget);
	m_layout->insertWidget(m_layout->count()-1, widget);
	widget->show();
	connect(widget, SIGNAL(modifiedChanged(bool)), SLOT(onModifiedChanged(bool)));
}

void ContactListSettings::loadImpl()
{
	m_modified = false;
	foreach (SettingsWidget *widget, m_extensionWidgets) {
		m_layout->removeWidget(widget);
		widget->hide();
		disconnect(widget, 0, this, 0);
	}
	m_extensionWidgets.clear();

	foreach (SettingsWidget *widget, m_staticExtensionWidgets)
		widget->load();

	QHash<QByteArray, const ObjectGenerator*>::iterator itr = m_extensions.begin(), end = m_extensions.end();
	while (itr != end) {
		if (m_services.contains(itr.key()))
			addExtensionWidget(itr.key(), itr.value(), true);
		++itr;
	}
}

void ContactListSettings::saveImpl()
{
	m_modified = false;
	bool showNotification = false;
	foreach (ServiceChooser *chooser, m_serviceChoosers) {
		QByteArray serviceName = chooser->service();
		ExtensionInfo info = chooser->currentServiceInfo();
		showNotification = !ServiceManager::setImplementation(serviceName, info) || showNotification;
	}
	if (showNotification) {
		NotificationRequest request(Notification::System);
		request.setText(tr("To take effect you must restart qutIM"));
		request.send();
	}

	foreach (SettingsWidget *widget, m_extensionWidgets)
		widget->save();
	foreach (SettingsWidget *widget, m_staticExtensionWidgets)
		widget->save();
}

void ContactListSettings::cancelImpl()
{
	m_modified = false;
	foreach (ServiceChooser *chooser, m_serviceChoosers) {
		QObject *serviceObj = ServiceManager::getByName(chooser->service());
		if (serviceObj)
			chooser->setCurrentService(serviceObj->metaObject()->className());
	}

	foreach (SettingsWidget *widget, m_widgetCache)
		widget->cancel();
}

void ContactListSettings::onServiceChanged(const QByteArray &newService, const QByteArray &oldService)
{
	foreach (SettingsWidget *widget, m_extensionWidgets.values(oldService)) {
		m_layout->removeWidget(widget);
		widget->hide();
		disconnect(widget, 0, this, 0);
	}
	m_extensionWidgets.remove(oldService);

	foreach (const ObjectGenerator *gen, m_extensions.values(newService))
		addExtensionWidget(newService, gen, false);

	m_services.remove(oldService);
	m_services.insert(newService);

	if (!m_modified) {
		m_modified = true;
		emit modifiedChanged(true);
	}
}

void ContactListSettings::onModifiedChanged(bool haveChanges)
{
	if (haveChanges && !m_modified) {
		m_modified = true;
		emit modifiedChanged(true);
	}
}

} // namespace Core
