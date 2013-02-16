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
#include "simplecontactlistsettings.h"
#include "ui_simplecontactlistsettings.h"
#include <qutim/config.h>
#include <qutim/localizedstring.h>
#include <qutim/status.h>
#include <qutim/protocol.h>
#include <qutim/servicemanager.h>
#include <QFontDialog>
#include <QDebug>

namespace Core
{

typedef QMap<int,LocalizedString> SizeMap;

void size_map_init(SizeMap &map)
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

SimpleContactlistSettings::SimpleContactlistSettings() :
    ui(new Ui::SimpleContactlistSettings)
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
    listenChildrenStates();
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
    connect(ui->headerFontSettingsButton,SIGNAL(clicked()),SLOT(headerFontSettings()));
    connect(ui->contactFontSettingsButton,SIGNAL(clicked()),SLOT(contactFontSettings()));
    Config config = Config("appearance").group("contactList");
    ui->avatarsBox->setChecked(config.value("showAvatars", true));
    ui->extendedInfoBox->setChecked(config.value("showExtendedInfoIcons", true));
    ui->statusBox->setChecked(config.value("showStatusText", true));
    ui->liteBox->setChecked(config.value("liteMode", true));
    ui->onstartupBox->setChecked(config.value("showContactListOnStartup", true));
    m_headerFont.fromString(config.value("HeaderFont").toString());
    m_contactFont.fromString(config.value("ContactFont").toString());
    setButtonText(m_headerFont, ui->headerFontSettingsButton);
    setButtonText(m_contactFont,ui->contactFontSettingsButton);
    // Load extendes statuses
    config.beginGroup("extendedStatuses");
    foreach (QCheckBox *checkBox, m_statusesBoxes) {
        bool checked = config.value(checkBox->objectName(), true);
        checkBox->setChecked(checked);
    }
    config.endGroup();

    int size = config.value("statusIconSize",0);
    SizeMap::const_iterator it;
    int index = -1;
    for (int i = 0;i!=ui->sizesBox->count();++i) {
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

void SimpleContactlistSettings::saveImpl()
{
    Config config = Config("appearance").group("contactList");
    config.setValue("showStatusText", ui->statusBox->isChecked());
    config.setValue("showExtendedInfoIcons", ui->extendedInfoBox->isChecked());
    config.setValue("showAvatars", ui->avatarsBox->isChecked());
    config.setValue("liteMode", ui->liteBox->isChecked());
    config.setValue("showContactListOnStartup", ui->onstartupBox->isChecked());
    int size = ui->sizesBox->itemData(ui->sizesBox->currentIndex()).toInt();
    config.setValue("HeaderFont",m_headerFont.toString());
    config.setValue("ContactFont",m_contactFont.toString());
    if (size == 0)
        config.remove("statusIconSize");
    else
        config.setValue("statusIconSize",size);
    // Save extended statuses
    config.beginGroup("extendedStatuses");
    foreach (QCheckBox *checkBox, m_statusesBoxes)
        config.setValue(checkBox->objectName(), checkBox->isChecked());
    config.endGroup();

    if (ContactDelegate *delegate = ServiceManager::getByName<ContactDelegate*>("ContactDelegate"))
        delegate->reloadSettings();
}

void SimpleContactlistSettings::reloadCombobox()
{
    ui->sizesBox->clear();
    SizeMap sizeMap;
    size_map_init(sizeMap);
    SizeMap::const_iterator it;
    for (it = sizeMap.constBegin();it!=sizeMap.constEnd();++it) {
        ui->sizesBox->addItem(it->toString());
        ui->sizesBox->setItemData(ui->sizesBox->count()-1,it.key());
    }
}

void SimpleContactlistSettings::headerFontSettings()
{
    bool ok;
    m_headerFont = QFontDialog::getFont(&ok,m_headerFont, this);
    if (ok) {
    saveImpl();
    setButtonText(m_headerFont, ui->headerFontSettingsButton);
    }
}

void SimpleContactlistSettings::contactFontSettings()
{
    bool ok;
    m_contactFont = QFontDialog::getFont(&ok,m_contactFont, this);
    if (ok) {
    saveImpl();
    setButtonText(m_contactFont,ui->contactFontSettingsButton);
    }
}
void SimpleContactlistSettings::setButtonText(QFont font, QPushButton *button)
{
    button->setText
            (tr("%1 %2").arg( font.family()).arg( font.pointSize()));
    button->setFont(font);
}
}

