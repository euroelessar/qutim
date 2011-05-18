#include "olddelegatesettings.h"
#include "ui_olddelegatesettings.h"
#include <qutim/config.h>
#include <qutim/localizedstring.h>
#include <qutim/status.h>
#include <qutim/protocol.h>
#include <qutim/systeminfo.h>
#include <qutim/thememanager.h>

typedef QMap<int,LocalizedString> SizeMap;

static void size_map_init(SizeMap &map)
{
	//TODO rewrite!

	//map.insert(-1,QT_TRANSLATE_NOOP("ContactList","Other"));
	map.insert(0,QT_TRANSLATE_NOOP("ContactList","Default (depends on platform)"));
	map.insert(16,QT_TRANSLATE_NOOP("ContactList","Small (16x16)"));
	map.insert(22,QT_TRANSLATE_NOOP("ContactList","Medium (22x22)"));
	map.insert(32,QT_TRANSLATE_NOOP("ContactList","Large (32x32)"));
	map.insert(48,QT_TRANSLATE_NOOP("ContactList","Very large (48x48)"));
	map.insert(64,QT_TRANSLATE_NOOP("ContactList","Huge (64x64)"));
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

#if 1 // FIXME: the option does not work well yet
	ui->sizesBox->setVisible(false);
	ui->label->setVisible(false);
#endif
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
	reloadCombobox();
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

	int size = config.value("statusIconSize", 0);
	int index = -1;
	for (int i = 0; i!=ui->sizesBox->count(); ++i) {
		if (size == ui->sizesBox->itemData(i).toInt()) {
			index = i;
			break;
		}
	}
	if (index == -1) {
		index = 0;
		//other size (TODO)
	}
	else
		ui->sizesBox->setCurrentIndex(index);
}

void OldDelegateSettings::saveImpl()
{
	Config config = Config("appearance").group("contactList");
	config.setValue("showStatusText", ui->statusBox->isChecked());
	config.setValue("showExtendedInfoIcons", ui->extendedInfoBox->isChecked());
	config.setValue("showAvatars", ui->avatarsBox->isChecked());
	int size = ui->sizesBox->itemData(ui->sizesBox->currentIndex()).toInt();
	if (size == 0)
		config.remove("statusIconSize");
	else
		config.setValue("statusIconSize",size);
	// Save extended statuses
	config.beginGroup("extendedStatuses");
	foreach (QCheckBox *checkBox, m_statusesBoxes)
		config.setValue(checkBox->objectName(), checkBox->isChecked());
	config.setValue("oldDelegateTheme", ui->themeBox->currentText());
	config.endGroup();

	if (ContactListItemDelegate *delegate = ServiceManager::getByName<ContactListItemDelegate*>("ContactDelegate"))
		delegate->reloadSettings();
}

void OldDelegateSettings::reloadCombobox()
{
	ui->sizesBox->clear();
	SizeMap sizeMap;
	size_map_init(sizeMap);
	SizeMap::const_iterator it;
	for (it = sizeMap.constBegin();it!=sizeMap.constEnd();it++) {
		ui->sizesBox->addItem(it->toString());
		ui->sizesBox->setItemData(ui->sizesBox->count()-1,it.key());
	}
}
