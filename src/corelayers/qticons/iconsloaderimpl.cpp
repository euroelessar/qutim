#include "iconsloaderimpl.h"
#include <qutim/configbase.h>
#include "3rdparty/q-xdg/src/xdgiconmanager.h"
#include <qutim/systeminfo.h>
#include <qutim/settingslayer.h>
#include <qutim/debug.h>
#include <QFormLayout>

namespace Core
{
Q_GLOBAL_STATIC_WITH_ARGS(XdgIconManager, iconManager,
						  (QList<QDir>()
						   << SystemInfo::getDir(SystemInfo::ShareDir)
						   << SystemInfo::getDir(SystemInfo::SystemShareDir)))

IconLoaderSettings::IconLoaderSettings()
{
	m_box = new QComboBox(this);
	QFormLayout *layout = new QFormLayout(this);
	layout->addRow(tr("Theme"), m_box);
	connect(m_box, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void IconLoaderSettings::loadImpl()
{
	m_box->clear();
	m_index = -1;
	QString themeId = Config().group("appearance").value("theme", QString());
	foreach (const QString &id, iconManager()->themeIds(false)) {
		const XdgIconTheme *theme = iconManager()->themeById(id);
		m_box->addItem(QIcon(), theme->name(), theme->id());
		if (themeId == id)
			m_index = m_box->count() - 1;
	}
	m_box->setCurrentIndex(m_index);
}

void IconLoaderSettings::saveImpl()
{
	QString themeId = m_box->itemData(m_box->currentIndex()).toString();
	Config().group("appearance").setValue("theme", themeId);
	iconManager()->setCurrentTheme(themeId);
}

void IconLoaderSettings::cancelImpl()
{
	loadImpl();
}

void IconLoaderSettings::onCurrentIndexChanged(int index)
{
	emit modifiedChanged(index != m_index);
}

IconLoaderImpl::IconLoaderImpl()
{
	onSettingsChanged();
	SettingsItem *item = new GeneralSettingsItem<IconLoaderSettings>(Settings::Appearance, QIcon(),
	                                                                 QT_TRANSLATE_NOOP("Settings", "Icons theme"));
//	item->setConfig(QString(), QLatin1String("appearance"));
//	AutoSettingsItem::Entry *entry = item->addEntry<ThemeBox>(QT_TRANSLATE_NOOP("Settings", "Current theme"));
//	entry->setName(QLatin1String("theme"));
//	entry->setProperty("items", iconManager()->themeIds(false));
//	debug() << "ICONS:" << iconManager()->themeIds(true);
//	debug() << "ICONS:" << iconManager()->themeIds(false);
	Settings::registerItem(item);
}

QIcon IconLoaderImpl::loadIcon(const QString &name)
{
	return iconManager()->getIcon(name);
}

QMovie *IconLoaderImpl::loadMovie(const QString &name)
{
	Q_UNUSED(name);
	return 0;
}

QString IconLoaderImpl::iconPath(const QString &name, uint iconSize)
{
	return iconManager()->currentTheme()->getIconPath(name, iconSize);
}

QString IconLoaderImpl::moviePath(const QString &name, uint iconSize)
{
	Q_UNUSED(name);
	Q_UNUSED(iconSize);
	return QString();
}

void IconLoaderImpl::onSettingsChanged()
{
	const XdgIconTheme *defTheme = iconManager()->defaultTheme();
	QString id = Config().group("appearance").value<QString>("theme", QString());
	const XdgIconTheme *theme;
	theme = iconManager()->themeById(id);
	if (!theme && defTheme && defTheme->id() != "hicolor") {
		theme = defTheme;
	} else if (!theme) {
		theme = iconManager()->themeById(QLatin1String("oxygen"));
		if (!theme)
			theme = iconManager()->themeById(QLatin1String("hicolor"));
	}
	// We don't want usually to use "hicolor"
	if (theme->id() == "hicolor") {
		QStringList themes = iconManager()->themeIds();
		themes.removeOne("hicolor");
		if (!themes.isEmpty())
			theme = iconManager()->themeById(themes.at(qrand() % themes.size()));
	}
	iconManager()->setCurrentTheme(theme->id());
}
}
