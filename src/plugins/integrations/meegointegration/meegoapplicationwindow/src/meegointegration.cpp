/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "meegointegration.h"
#include "quicksettingslayer.h"
#include <QDebug>

namespace MeegoIntegration {
using namespace qutim_sdk_0_3;

MeeGoIntegration::MeeGoIntegration()
{
}

void MeeGoIntegration::init()
{
}

bool MeeGoIntegration::isAvailable() const
{
	return true;
}

int MeeGoIntegration::priority()
{
	return DesktopEnvironment;
}

QVariant MeeGoIntegration::doGetValue(SystemIntegration::Attribute attr, const QVariant &data) const
{
	Q_UNUSED(attr);
	Q_UNUSED(data);
	return QVariant();
}

class SettingsItemHook : public SettingsItem
{
public:
	using SettingsItem::generator;
	static ObjectGenerator *generator(SettingsItem *item)
	{
		const ObjectGenerator *gen = static_cast<SettingsItemHook*>(item)->generator();
		return const_cast<ObjectGenerator*>(gen);
	}
};

QVariant MeeGoIntegration::doProcess(SystemIntegration::Operation act, const QVariant &data) const
{
	switch (act) {
	case OpenDialog:
	case ShowWidget: {
		QWidget *widget = data.value<QWidget*>();
		m_window->showWidget(widget);
		return QVariant();
	}
	case GetSettingsGenerator: {
		SettingsItem *item = data.value<SettingsItem*>();
		const QByteArray name = item->text().original();
		ObjectGenerator *generator = NULL;
		QScopedPointer<ObjectGenerator> gen(SettingsItemHook::generator(item));
		const QMetaObject *meta = gen->metaObject();
		QLatin1String className = QLatin1String(meta->className());
		if (className == QLatin1String("Jabber::JMainSettings"))
			generator = new QuickGenerator(QLatin1String("accounts/JabberPage.qml"));
		else if (className == QLatin1String("VAccountSettings"))
			generator = new QuickGenerator(QLatin1String("accounts/VKPage.qml"));
		else if (className == QLatin1String("qutim_sdk_0_3::DataSettingsWidget"))
			generator = new QuickGenerator(QLatin1String("settings/DataItemPage.qml"),
										   new DataSettingsObjectCreator(item));
		else if (name == "Auto-away")
			generator = new QuickGenerator(QLatin1String("settings/AutoAwayPage.qml"));
		else if (name == "Icq account settings")
			generator = new QuickGenerator(QLatin1String("accounts/OscarPage.qml"));
		else if (name == "Connection manager")
			generator = new QuickGenerator(QLatin1String("settings/ConnectionManagerPage.qml"));
		else if (name == "BlogImprover")
			generator = new QuickGenerator(QLatin1String("settings/BlogImproverPage.qml"));
		else if (name == "Highlighter")
			generator = new QuickGenerator(QLatin1String("settings/HighlighterPage.qml"));
		else if (name == "Localization")
			generator = new QuickGenerator(QLatin1String("settings/LanguagesPage.qml"));
		else if (name == "Sounds")
			generator = new QuickGenerator(QLatin1String("settings/SoundThemePage.qml"));
		else if (name == "Notifications")
			generator = new QuickGenerator(QLatin1String("settings/NotificationsPage.qml"));
		else if (name == "UrlPreview")
			generator = new QuickGenerator(QLatin1String("settings/UrlPreviewPage.qml"));
		else if (name == "Chat")
			generator = new QuickGenerator(QLatin1String("settings/ChatPage.qml"));
		else if (name == "Antispam")
			generator = new QuickGenerator(QLatin1String("settings/AntispamPage.qml"));
		else if (name == "Global proxy" || name == "Proxy")
			generator = new QuickGenerator(QLatin1String("settings/ProxyPage.qml"));
		else if (name == "Emoticons")
			generator = new QuickGenerator(QLatin1String("settings/EmoticonsPage.qml"));
		return qVariantFromValue(generator ? generator : gen.take());
	}
	default:
		break;
	}
	return QVariant();
}

bool MeeGoIntegration::canHandle(SystemIntegration::Attribute attribute) const
{
	Q_UNUSED(attribute);
	return false;
}

bool MeeGoIntegration::canHandle(SystemIntegration::Operation operation) const
{
	switch (operation) {
	case ShowWidget:
	case GetSettingsGenerator:
	case KeepAliveSocket:
	case OpenDialog:
		return true;
	default:
		return false;
	}
}

}

