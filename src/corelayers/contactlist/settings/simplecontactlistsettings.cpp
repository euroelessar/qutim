#include "simplecontactlistsettings.h"
#include "ui_simplecontactlistsettings.h"
#include <libqutim/config.h>
#include <libqutim/localizedstring.h>

namespace Core
{
namespace SimpleContactList
{

typedef QMap<int,LocalizedString> SizeMap;

void size_map_init(SizeMap &map)
{
	//map.insert(-1,QT_TRANSLATE_NOOP("ContactList","Other"));
	map.insert(0,QT_TRANSLATE_NOOP("ContactList","Default (depends on platform)"));
	map.insert(16,QT_TRANSLATE_NOOP("ContactList","Small (16x16)"));
	map.insert(22,QT_TRANSLATE_NOOP("ContactList","Medium (22x22)"));
	map.insert(32,QT_TRANSLATE_NOOP("ContactList","Large (32x32)"));
	map.insert(48,QT_TRANSLATE_NOOP("ContactList","Very large (48x48)"));
	map.insert(64,QT_TRANSLATE_NOOP("ContactList","Hige (64x64)"));
}

SimpleContactlistSettings::SimpleContactlistSettings() :
		ui(new Ui::SimpleContactlistSettings)
{
	ui->setupUi(this);
	connect(ui->avatarsBox,SIGNAL(toggled(bool)),SLOT(onAvatarBoxToggled(bool)));
	connect(ui->extendedInfoBox,SIGNAL(toggled(bool)),SLOT(onExtendedInfoBoxToggled(bool)));
	connect(ui->statusBox,SIGNAL(toggled(bool)),SLOT(onStatusBoxToggled(bool)));
	connect(ui->sizesBox,SIGNAL(currentIndexChanged(int)),SLOT(onSizesCurrentIndexChanged(int)));
}

SimpleContactlistSettings::~SimpleContactlistSettings()
{
	delete ui;
}

void SimpleContactlistSettings::cancelImpl()
{

}

void SimpleContactlistSettings::loadImpl()
{
	reloadCombobox();
	Config config = Config("appearance").group("contactList");
	m_flags = static_cast<Delegate::ShowFlags>(config.value("showFlags",Delegate::ShowStatusText |
																			 Delegate::ShowExtendedInfoIcons |
																			 Delegate::ShowAvatars));
	int size = config.value("iconSize",0);
	SizeMap::const_iterator it;
	int index = -1;
	for (int i = 0;i!=ui->sizesBox->count();i++) {
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
	ui->avatarsBox->setChecked(m_flags & Delegate::ShowAvatars);
	ui->extendedInfoBox->setChecked(m_flags & Delegate::ShowExtendedInfoIcons);
	ui->statusBox->setChecked(m_flags & Delegate::ShowStatusText);
}

void SimpleContactlistSettings::saveImpl()
{
	Config config = Config("appearance").group("contactList");
	config.setValue("showFlags",m_flags);
	int size = ui->sizesBox->itemData(ui->sizesBox->currentIndex()).toInt();
	if (size == 0) {
		config.remove("iconSize");
	}
	else
		config.setValue("iconSize",size);
	config.sync();	
}

void SimpleContactlistSettings::reloadCombobox()
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

void SimpleContactlistSettings::setFlag(Delegate::ShowFlags flag, bool on)
{
	if (on) 
		m_flags |= flag;
	else 
		m_flags &= ~flag;
}

void SimpleContactlistSettings::onAvatarBoxToggled(bool toggled)
{
	setFlag(Delegate::ShowAvatars,toggled);
	emit modifiedChanged(true);
}
void SimpleContactlistSettings::onExtendedInfoBoxToggled(bool toggled)
{
	setFlag(Delegate::ShowExtendedInfoIcons,toggled);
	emit modifiedChanged(true);
}
void SimpleContactlistSettings::onStatusBoxToggled(bool toggled)
{
	setFlag(Delegate::ShowStatusText,toggled);	
	emit modifiedChanged(true);
}

void SimpleContactlistSettings::onSizesCurrentIndexChanged(int index)
{
	emit modifiedChanged(true);
}


}
}
