/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "simplecontactlistsettings.h"
#include <qutim/metaobjectbuilder.h>
#include <qutim/notification.h>
#include <qutim/objectgenerator.h>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSet>
#include <QCoreApplication>

namespace Core {

ContactListSettings::ContactListSettings()
{
	m_layout = new QVBoxLayout(this);

    addService("ContactComparator", QT_TR_NOOP("Sorting"));
	addService("ContactModel", QT_TR_NOOP("Model"));
	addService("ContactListWidget", QT_TR_NOOP("Widget style"));
	addService("ContactDelegate", QT_TR_NOOP("Contacts style"));

	m_layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));

	foreach(const ObjectGenerator *gen, ObjectGenerator::module<ContactListSettingsExtention>()) {
		QByteArray service = MetaObjectBuilder::info(gen->metaObject(), "ServiceSettings");
		if (service.isEmpty())
			addExtensionWidget(QByteArray(), gen, false);
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
	QByteArray currentService = serviceObj->metaObject()->className();
	if (services.size() > 1) {
		m_services.insert(currentService);
		ServiceChooser *chooser = new ServiceChooser(service, title, currentService, services, this);
		m_layout->addWidget(chooser);
		m_serviceChoosers.insert(service, chooser);
		connect(chooser,
				SIGNAL(serviceChanged(QByteArray,QByteArray)),
				SLOT(onServiceChanged(QByteArray,QByteArray)));
	} else {
		onServiceChanged(currentService, QByteArray());
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

