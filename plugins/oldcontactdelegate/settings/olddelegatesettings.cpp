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
#include "olddelegatesettings.h"
#include "ui_olddelegatesettings.h"
#include <qutim/config.h>
#include <qutim/localizedstring.h>
#include <qutim/status.h>
#include <qutim/protocol.h>
#include <qutim/systeminfo.h>
#include <qutim/thememanager.h>
#include <qutim/servicemanager.h>

typedef QMap<int,LocalizedString> Sizes;

static inline const Sizes &getSizeMap()
{
	static Sizes map;
	if (map.isEmpty()) {
		map.insert(0,  QT_TRANSLATE_NOOP("ContactList","Default (depends on platform)"));
		map.insert(16, QT_TRANSLATE_NOOP("ContactList","Small (16x16)"));
		map.insert(22, QT_TRANSLATE_NOOP("ContactList","Medium (22x22)"));
		map.insert(32, QT_TRANSLATE_NOOP("ContactList","Large (32x32)"));
		map.insert(48, QT_TRANSLATE_NOOP("ContactList","Very large (48x48)"));
		map.insert(64, QT_TRANSLATE_NOOP("ContactList","Huge (64x64)"));
	}
	return map;
}

OldDelegateSettings::OldDelegateSettings() :
		ui(new Ui::OldDelegateSettings)
{
	ui->setupUi(this);
	// Load extended statuses list
	foreach (Protocol *protocol, Protocol::all()) {
		ExtendedInfosEvent event;
		qApp->sendEvent(protocol, &event);
		foreach (const QVariantHash &info, event.infos()) {
			QString id = info.value("id").toString();
			if (id.isEmpty() || m_statusesBoxes.contains(id))
				continue;
			QString desc = info.value("settingsDescription").toString();
			if (desc.isEmpty())
				desc = QString("Show '%' icon").arg(id);
			QCheckBox *box = new QCheckBox(desc, this);
			box->setObjectName(id);
			connect(ui->extendedInfoBox, SIGNAL(toggled(bool)), box, SLOT(setEnabled(bool)));
			ui->layout->addRow(box);
			m_statusesBoxes.insert(id, box);
		}
	}

	initCombobox(ui->statusSizesBox);
	initCombobox(ui->avatarSizesBox);
	listenChildrenStates();
}

OldDelegateSettings::~OldDelegateSettings()
{
	delete ui;
}

void OldDelegateSettings::cancelImpl()
{

}

static inline QStringList listThemes(QDir shareDir)
{
	if (shareDir.cd("olddelegate"))
		return shareDir.entryList(QStringList() << "*.ListQutim" << "*.ListTheme", QDir::NoDotAndDotDot);
	else
		return QStringList();
}

void OldDelegateSettings::loadImpl()
{
	Config config = Config("appearance").group("contactList");
	ui->avatarsBox->setChecked(config.value("showAvatars", true));
	ui->extendedInfoBox->setChecked(config.value("showExtendedInfoIcons", true));
	ui->statusBox->setChecked(config.value("showStatusText", true));

	// Load theme names
	QString current = config.value("oldDelegateTheme", "default.ListQutim");
	foreach (QDir dir, ThemeManager::categoryDirs("olddelegate")) {
		QStringList themes = dir.entryList(QStringList() << "*.ListQutim" << "*.ListTheme",
										   QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
		ui->themeBox->addItems(themes);
	}
	ui->themeBox->setCurrentIndex(ui->themeBox->findText(current));

	// Load extendes statuses
	config.beginGroup("extendedStatuses");
	foreach (QCheckBox *checkBox, m_statusesBoxes) {
		bool checked = config.value(checkBox->objectName(), true);
		checkBox->setChecked(checked);
	}
	config.endGroup();

	setSize(ui->statusSizesBox, config.value("statusIconSize", 0));
	setSize(ui->avatarSizesBox, config.value("avatarIconSize", 0));
}

void OldDelegateSettings::saveImpl()
{
	Config config = Config("appearance").group("contactList");
	config.setValue("showStatusText", ui->statusBox->isChecked());
	config.setValue("showExtendedInfoIcons", ui->extendedInfoBox->isChecked());
	config.setValue("showAvatars", ui->avatarsBox->isChecked());
	storeSizeToConfig(ui->statusSizesBox, config, "statusIconSize");
	storeSizeToConfig(ui->avatarSizesBox, config, "avatarIconSize");
	// Save extended statuses
	config.beginGroup("extendedStatuses");
	foreach (QCheckBox *checkBox, m_statusesBoxes)
		config.setValue(checkBox->objectName(), checkBox->isChecked());
	config.setValue("oldDelegateTheme", ui->themeBox->currentText());
	config.endGroup();

	if (ContactListItemDelegate *delegate = ServiceManager::getByName<ContactListItemDelegate*>("ContactDelegate"))
		delegate->reloadSettings();
}

void OldDelegateSettings::initCombobox(QComboBox *box)
{
	const Sizes &map = getSizeMap();
	Sizes::const_iterator itr = map.constBegin();
	Sizes::const_iterator end = map.constEnd();
	for (; itr != end; ++itr) {
		box->addItem(itr->toString());
		box->setItemData(box->count()-1, itr.key());
	}
}

void OldDelegateSettings::setSize(QComboBox *box, int size)
{
	int index = 0;
	for (int i = 0, n = box->count(); i != n; ++i) {
		if (size == box->itemData(i).toInt()) {
			index = i;
			break;
		}
	}
	box->setCurrentIndex(index);
}

void OldDelegateSettings::storeSizeToConfig(QComboBox *box, Config &config, const char *value)
{
	int size = box->itemData(box->currentIndex()).toInt();
	if (size == 0)
		config.remove(value);
	else
		config.setValue(value, size);
}

