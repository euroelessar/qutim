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
	case ShowWidget: {
		QWidget *widget = data.value<QWidget*>();
		m_window->showWidget(widget);
		return QVariant();
	}
	case GetSettingsGenerator: {
		SettingsItem *item = data.value<SettingsItem*>();
		const QString iconName = item->icon().name();
		const QByteArray name = item->text().original();
		ObjectGenerator *generator = NULL;
		if (name == "Auto-away") {
			generator = new QuickGenerator(QLatin1String("settings/AutoAwayPage.qml"));
		} else if (name == "Icq account settings") {
			generator = new QuickGenerator(QLatin1String("accounts/OscarPage.qml"));
		} else if (name == "Connection manager") {
			generator = new QuickGenerator(QLatin1String("settings/ConnectionManagerPage.qml"));
		} else if (name == "BlogImprover") {
			generator = new QuickGenerator(QLatin1String("settings/BlogImproverPage.qml"));
		} else if (name == "Highlighter") {
			generator = new QuickGenerator(QLatin1String("settings/HighlighterPage.qml"));
		} else if (name == "Localization") {
			generator = new QuickGenerator(QLatin1String("settings/LanguagesPage.qml"));
		} else if (name == "Main settings") {
			QScopedPointer<ObjectGenerator> gen(SettingsItemHook::generator(item));
			const QMetaObject *meta = gen->metaObject();
			if (!qstrcmp(meta->className(), "Jabber::JMainSettings"))
				generator = new QuickGenerator(QLatin1String("accounts/JabberPage.qml"));
		}
		return qVariantFromValue(generator);
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
		return true;
	default:
		return false;
	}
}

}

