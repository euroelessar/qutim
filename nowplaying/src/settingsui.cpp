#include "settingsui.h"
#include "player.h"
#include <qutim/objectgenerator.h>

namespace qutim_sdk_0_3 {
namespace nowplaying{

SettingsUI::SettingsUI(){
    ui.setupUi(this);

}

SettingsUI::~SettingsUI(){

}

void SettingsUI::loadImpl(){
    QString proto_id;
    QString acc_id;
    m_accounts.insert("oscar", "icq");
    m_accounts.insert("mrim", "mrim");
    m_accounts.insert("jabber", "jabber");
    foreach (Protocol* proto, allProtocols()) {
        foreach(Account* acc, proto->accounts()){
            proto_id = proto->id();
            acc_id = acc->id();
            m_accounts.insert(acc_id, proto_id);
            ui.accounts->addItem(acc_id);
        }
    }
    Config group = Config().group("nowplaying");
    Config global = group.group("global");
    m_is_working = global.value("is_working", false);
    ui.players->setCurrentIndex(ui.players->findText(global.value("player", QString("Amarok"))));
    ui.for_all_accounts->setChecked(global.value("for_all_accounts", true));

    foreach (QString acc, m_accounts.keys()){
        QString proto_id = m_accounts.value(acc);
        if (proto_id == "icq"){
            Config saved_config = group.group(acc);
            Oscar config;
            config.deactivated = saved_config.value("deactivated", false);
            config.sets_current_status = saved_config.value("sets_current_status", false);
            config.sets_music_status = saved_config.value("sets_music_status", true);
            config.mask_1 = saved_config.value("mask_1", QString("Now playing: %artist - %title"));
            config.mask_2 = saved_config.value("mask_2", QString("%artist - %title"));
            m_oscar_accs.insert(acc, config);
        } else if (proto_id == "mrim"){
            Config saved_config = group.group(acc);
            MRIM config;
            config.deactivated = saved_config.value("deactivated", false);
            config.sets_current_status = saved_config.value("sets_current_status", false);
            config.sets_music_status = saved_config.value("sets_music_status", true);
            config.mask_1 = saved_config.value("mask_1", QString("Now playing: %artist - %title"));
            config.mask_2 = saved_config.value("mask_2", QString("%artist - %title"));
            m_mrim_accs.insert(acc, config);
        } else if (proto_id == "jabber"){
            Config saved_config = group.group(acc);
            Jabber config;
            config.deactevated = saved_config.value("deactevated", false);
            config.artist = saved_config.value("artist", true);
            config.title = saved_config.value("title", true);
            config.album = saved_config.value("album", false);
            config.length = saved_config.value("length", false);
            config.number = saved_config.value("number", false);
            config.uri = saved_config.value("uri", false);
            m_jabber_accs.insert(acc, config);
        }
    }

    m_current_account = ui.accounts->currentText();
    initFormValues();
    connect(ui.accounts, SIGNAL(currentIndexChanged(QString)), this, SLOT(accountChanged(QString)));
    connect(ui.change_status, SIGNAL(clicked()), this, SLOT(stopButtonClicked()));
    connect(ui.for_all_accounts, SIGNAL(toggled(bool)), this, SLOT(forAllAccsClicked()));
    lookForWidgetState(ui.for_all_accounts);
    lookForWidgetState(ui.oscar_deactevated);
    lookForWidgetState(ui.oscar_change_current);
    lookForWidgetState(ui.oscar_change_music_status);
    lookForWidgetState(ui.oscar_mask_1);
    lookForWidgetState(ui.oscar_mask_2);
    lookForWidgetState(ui.mrim_deactevated);
    lookForWidgetState(ui.mrim_change_current);
    lookForWidgetState(ui.mrim_change_music_status);
    lookForWidgetState(ui.mrim_mask_1);
    lookForWidgetState(ui.mrim_mask_2);
    lookForWidgetState(ui.jabber_deactivated);
    lookForWidgetState(ui.jabber_activated);
    lookForWidgetState(ui.jabber_album);
    lookForWidgetState(ui.jabber_artist);
    lookForWidgetState(ui.jabber_length);
    lookForWidgetState(ui.jabber_title);
    lookForWidgetState(ui.jabber_track_number);
    lookForWidgetState(ui.jabber_uri);

    foreach(const ObjectGenerator *gen, moduleGenerators<Player>()){
        Player* player =  gen->generate<Player>();
        QString name = player->playerName();
        if (ui.players->findText(name) == -1)
            ui.players->addItem(name);
    }

}

void SettingsUI::saveImpl(){
    if (ui.for_all_accounts->isChecked()){
        saveOscarFormFields("oscar");
        saveMRIMFormFields("mrim");
        saveJabberFormFields("jabber");
    } else{
        saveAccountFormField(ui.accounts->currentText());
    }
    Config group = Config().group("nowplaying");
    Config global = group.group("global");
    global.setValue("is_working", m_is_working);
    global.setValue("player", ui.players->currentText());
    global.setValue("for_all_accounts", ui.for_all_accounts->isChecked());

    foreach (QString acc, m_oscar_accs.keys()){
        Config saved_config = group.group(acc);
        Oscar config = m_oscar_accs.value(acc);
        saved_config.setValue("deactivated", config.deactivated);
        saved_config.setValue("sets_current_status", config.sets_current_status);
        saved_config.setValue("sets_music_status", config.sets_music_status);
        saved_config.setValue("mask_1", config.mask_1);
        saved_config.setValue("mask_2", config.mask_2);
    }

    foreach (QString acc, m_mrim_accs.keys()){
        Config saved_config = group.group(acc);
        MRIM config = m_mrim_accs.value(acc);
        saved_config.setValue("deactivated", config.deactivated);
        saved_config.setValue("sets_current_status", config.sets_current_status);
        saved_config.setValue("sets_music_status", config.sets_music_status);
        saved_config.setValue("mask_1", config.mask_1);
        saved_config.setValue("mask_2", config.mask_2);
    }

    foreach (QString acc, m_jabber_accs.keys()){
        Config saved_config = group.group(acc);
        Jabber config = m_jabber_accs.value(acc);
        saved_config.setValue("deactevated", config.deactevated);
        saved_config.setValue("artist", config.artist);
        saved_config.setValue("title", config.title);
        saved_config.setValue("album", config.album);
        saved_config.setValue("length", config.length);
        saved_config.setValue("number", config.number);
        saved_config.setValue("uri", config.uri);
    }
    group.sync();
    emit configSaved();
}

void SettingsUI::cancelImpl(){
}

void SettingsUI::initFormValues(){
    if (m_is_working){
        ui.status_text->setText(" " + tr("working"));
        ui.change_status->setText(tr("Stop"));
    } else{
        ui.status_text->setText(" " + tr("not working"));
        ui.change_status->setText(tr("Start"));
    }
    if (!m_accounts.size()){
        ui.mode_settigs->setCurrentWidget(ui.no_accounts);
        return;
    }
    QString account_id = ui.accounts->currentText();
    if (!ui.for_all_accounts->isChecked()){
        ui.protocols->setTabEnabled(0, m_accounts.value(account_id) == "icq");
        ui.protocols->setTabEnabled(1, m_accounts.value(account_id) == "mrim");
        ui.protocols->setTabEnabled(2, m_accounts.value(account_id) == "jabber");
    } else {
        ui.protocols->setTabEnabled(0, m_oscar_accs.size() > 1);
        ui.protocols->setTabEnabled(1, m_mrim_accs.size() > 1);
        ui.protocols->setTabEnabled(2, m_jabber_accs.size() > 1);
        account_id = "global";
    }
    if (m_accounts.value((account_id == "global") ? "oscar" : m_current_account) == "icq"){
        initOscarFormFields((account_id == "global") ? "oscar" : m_current_account);
    }
    if (m_accounts.value((account_id == "global") ? "mrim" : m_current_account) == "mrim"){
        initMRIMFormFields((account_id == "global") ? "mrim" : m_current_account);
    }
    if (m_accounts.value((account_id == "global") ? "jabber" : m_current_account) == "jabber"){
        initJabberFormFields((account_id == "global") ? "jabber" : m_current_account);
    }
}

void SettingsUI::initOscarFormFields(const QString& acc){
    Oscar config = m_oscar_accs.value(acc);
    ui.oscar_deactevated->setChecked(config.deactivated);
    ui.oscar_change_current->setChecked(config.sets_current_status);
    ui.oscar_change_music_status->setChecked(config.sets_music_status);
    ui.oscar_mask_1->setText(config.mask_1);
    ui.oscar_mask_2->setText(config.mask_2);
}

void SettingsUI::saveOscarFormFields(const QString& acc){
    Oscar config;
    config.deactivated = ui.oscar_deactevated->isChecked();
    config.sets_current_status = ui.oscar_change_current->isChecked();
    config.sets_music_status = ui.oscar_change_music_status->isChecked();
    config.mask_1 = ui.oscar_mask_1->text();
    config.mask_2 = ui.oscar_mask_2->text();
    m_oscar_accs.insert(acc, config);
}

void SettingsUI::initMRIMFormFields(const QString& acc){
    MRIM config = m_mrim_accs.value(acc);
    ui.mrim_deactevated->setChecked(config.deactivated);
    ui.mrim_change_current->setChecked(config.sets_current_status);
    ui.mrim_change_music_status->setChecked(config.sets_music_status);
    ui.mrim_mask_1->setText(config.mask_1);
    ui.mrim_mask_2->setText(config.mask_2);
}

void SettingsUI::saveMRIMFormFields(const QString& acc){
    MRIM config;
    config.deactivated = ui.mrim_deactevated->isChecked();
    config.sets_current_status = ui.mrim_change_current->isChecked();
    config.sets_music_status = ui.mrim_change_music_status->isChecked();
    config.mask_1 = ui.mrim_mask_1->text();
    config.mask_2 = ui.mrim_mask_2->text();
    m_mrim_accs.insert(acc, config);
}

void SettingsUI::initJabberFormFields(const QString& acc){
    Jabber config = m_jabber_accs.value(acc);
    ui.jabber_deactivated->setChecked(config.deactevated);
    ui.jabber_activated->setChecked(!(config.deactevated));
    ui.jabber_artist->setChecked(config.artist);
    ui.jabber_title->setChecked(config.title);
    ui.jabber_album->setChecked(config.album);
    ui.jabber_track_number->setChecked(config.number);
    ui.jabber_length->setChecked(config.length);
    ui.jabber_uri->setChecked(config.uri);
}

void SettingsUI::saveJabberFormFields(const QString& acc){
    Jabber config;
    config.deactevated = ui.jabber_deactivated->isChecked();
    config.artist = ui.jabber_artist->isChecked();
    config.title = ui.jabber_title->isChecked();
    config.album = ui.jabber_album->isChecked();
    config.number = ui.jabber_track_number->isChecked();
    config.length = ui.jabber_length->isChecked();
    config.uri = ui.jabber_uri->isChecked();
    m_jabber_accs.insert(acc, config);
}

void SettingsUI::initAccountFormField(const QString& acc){
    if (m_accounts.value(acc) == "icq"){
        initOscarFormFields(acc);
    }
    if (m_accounts.value(acc) == "mrim"){
        initMRIMFormFields(acc);
    }
    if (m_accounts.value(acc) == "jabber"){
        initJabberFormFields(acc);
    }
}

void SettingsUI::saveAccountFormField(const QString& acc){
    if (m_accounts.value(acc) == "icq"){
        saveOscarFormFields(acc);
    }
    if (m_accounts.value(acc) == "mrim"){
        saveMRIMFormFields(acc);
    }
    if (m_accounts.value(acc) == "jabber"){
        saveJabberFormFields(acc);
    }
}

void SettingsUI::accountChanged(QString acc){
    saveAccountFormField(m_current_account);
    m_current_account = acc;
    initFormValues();
}

void SettingsUI::stopButtonClicked(){
    if (m_is_working){
        m_is_working = false;
        ui.status_text->setText(tr("not working"));
        ui.change_status->setText(tr("Start"));
    } else{
        m_is_working = true;
        ui.status_text->setText(tr("working"));
        ui.change_status->setText(tr("Stop"));
    }
    Config group = Config().group("nowplaying");
    Config global = group.group("global");
    global.setValue("is_working", m_is_working);
    group.sync();
    emit statusChanged(m_is_working);
}

void SettingsUI::forAllAccsClicked(){
    if (ui.for_all_accounts->isChecked()){
        saveAccountFormField(m_current_account);
        initFormValues();
    } else{
        saveOscarFormFields("oscar");
        saveMRIMFormFields("mrim");
        saveJabberFormFields("jabber");
        initFormValues();
    }
}

}}
