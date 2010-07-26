#include "settingsui.h"

SettingsUI::SettingsUI(){
    ui.setupUi(this);

}

SettingsUI::~SettingsUI(){

}

void SettingsUI::loadImpl(){
    QString proto_id;
    QString acc_id;
    m_accounts.insert("oscar", "icq");
    m_oscar_accs.insert("oscar", new Oscar(this));
    m_accounts.insert("mrim", "mrim");
    m_mrim_accs.insert("mrim", new MRIM(this));
    m_accounts.insert("jabber", "jabber");
    m_jabber_accs.insert("jabber", new Jabber(this));
    foreach (Protocol* proto, allProtocols()) {
        foreach(Account* acc, proto->accounts()){
            proto_id = proto->id();
            acc_id = acc->id();
            m_accounts.insert(acc_id, proto_id);
            ui.accounts->addItem(acc_id);
            //qDebug()<<proto_id;
            if (proto_id == "icq"){
                m_oscar_accs.insert(acc_id, new Oscar(this));
            } else if (proto_id == "mrim"){
                m_mrim_accs.insert(acc_id, new MRIM(this));
            } else if (proto_id == "jabber"){
                m_jabber_accs.insert(acc_id, new Jabber(this));
            }
        }
    }
    Config group = Config().group("nowplaying");
    Config global = group.group("global");
    m_is_working = global.value("is_working", false);
    ui.players->setCurrentIndex(ui.players->findText(global.value("player", QString("Amarok"))));
    ui.time_period->setText(global.value("check_period", QString("1")));
    ui.for_all_accounts->setChecked(global.value("for_all_accounts", true));

    foreach (QString acc, m_oscar_accs.keys()){
        Config oscar = group.group(acc);
        m_oscar_accs.value(acc)->deactivated = oscar.value("deactivated", false);
        m_oscar_accs.value(acc)->sets_current_status = oscar.value("sets_current_status", false);
        m_oscar_accs.value(acc)->sets_music_status = oscar.value("sets_music_status", true);
        m_oscar_accs.value(acc)->mask_1 = oscar.value("mask_1", QString("Now playing: %artist - %title"));
        m_oscar_accs.value(acc)->mask_2 = oscar.value("mask_2", QString("%artist - %title"));
    }

    foreach (QString acc, m_mrim_accs.keys()){
        Config mrim = group.group(acc);
        m_mrim_accs.value(acc)->deactivated = mrim.value("deactivated", false);
        m_mrim_accs.value(acc)->sets_current_status = mrim.value("sets_current_status", false);
        m_mrim_accs.value(acc)->sets_music_status = mrim.value("sets_music_status", true);
        m_mrim_accs.value(acc)->mask_1 = mrim.value("mask_1", QString("Now playing: %artist - %title"));
        m_mrim_accs.value(acc)->mask_2 = mrim.value("mask_2", QString("%artist - %title"));
    }

    foreach (QString acc, m_jabber_accs.keys()){
        Config jabber = group.group(acc);
        m_jabber_accs.value(acc)->deactevated = jabber.value("deactevated", false);
        m_jabber_accs.value(acc)->artist = jabber.value("artist", true);
        m_jabber_accs.value(acc)->title = jabber.value("title", true);
        m_jabber_accs.value(acc)->album = jabber.value("album", false);
        m_jabber_accs.value(acc)->length = jabber.value("length", false);
        m_jabber_accs.value(acc)->number = jabber.value("number", false);
        m_jabber_accs.value(acc)->uri = jabber.value("uri", false);
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
    global.setValue("check_period",ui.time_period->text());
    global.setValue("for_all_accounts", ui.for_all_accounts->isChecked());

    foreach (QString acc, m_oscar_accs.keys()){
        Config oscar = group.group(acc);
        oscar.setValue("deactivated", m_oscar_accs.value(acc)->deactivated);
        oscar.setValue("sets_current_status", m_oscar_accs.value(acc)->sets_current_status);
        oscar.setValue("sets_music_status", m_oscar_accs.value(acc)->sets_music_status);
        oscar.setValue("mask_1", m_oscar_accs.value(acc)->mask_1);
        oscar.setValue("mask_2", m_oscar_accs.value(acc)->mask_2);
    }

    foreach (QString acc, m_mrim_accs.keys()){
        Config mrim = group.group(acc);
        mrim.setValue("deactivated", m_mrim_accs.value(acc)->deactivated);
        mrim.setValue("sets_current_status", m_mrim_accs.value(acc)->sets_current_status);
        mrim.setValue("sets_music_status", m_mrim_accs.value(acc)->sets_music_status);
        mrim.setValue("mask_1", m_mrim_accs.value(acc)->mask_1);
        mrim.setValue("mask_2", m_mrim_accs.value(acc)->mask_2);
    }

    foreach (QString acc, m_jabber_accs.keys()){
        Config jabber = group.group(acc);
        jabber.setValue("deactevated", m_jabber_accs.value(acc)->deactevated);
        jabber.setValue("artist", m_jabber_accs.value(acc)->artist);
        jabber.setValue("title", m_jabber_accs.value(acc)->title);
        jabber.setValue("album", m_jabber_accs.value(acc)->album);
        jabber.setValue("length", m_jabber_accs.value(acc)->length);
        jabber.setValue("number", m_jabber_accs.value(acc)->number);
        jabber.setValue("uri", m_jabber_accs.value(acc)->uri);
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
    ui.oscar_deactevated->setChecked(m_oscar_accs.value(acc)->deactivated);
    ui.oscar_change_current->setChecked(m_oscar_accs.value(acc)->sets_current_status);
    ui.oscar_change_music_status->setChecked(m_oscar_accs.value(acc)->sets_music_status);
    ui.oscar_mask_1->setText(m_oscar_accs.value(acc)->mask_1);
    ui.oscar_mask_2->setText(m_oscar_accs.value(acc)->mask_2);
}

void SettingsUI::saveOscarFormFields(const QString& acc){
    m_oscar_accs.value(acc)->deactivated = ui.oscar_deactevated->isChecked();
    m_oscar_accs.value(acc)->sets_current_status = ui.oscar_change_current->isChecked();
    m_oscar_accs.value(acc)->sets_music_status = ui.oscar_change_music_status->isChecked();
    m_oscar_accs.value(acc)->mask_1 = ui.oscar_mask_1->text();
    m_oscar_accs.value(acc)->mask_2 = ui.oscar_mask_2->text();
}

void SettingsUI::initMRIMFormFields(const QString& acc){
    ui.mrim_deactevated->setChecked(m_mrim_accs.value(acc)->deactivated);
    ui.mrim_change_current->setChecked(m_mrim_accs.value(acc)->sets_current_status);
    ui.mrim_change_music_status->setChecked(m_mrim_accs.value(acc)->sets_music_status);
    ui.mrim_mask_1->setText(m_mrim_accs.value(acc)->mask_1);
    ui.mrim_mask_2->setText(m_mrim_accs.value(acc)->mask_2);
}

void SettingsUI::saveMRIMFormFields(const QString& acc){
    m_mrim_accs.value(acc)->deactivated = ui.mrim_deactevated->isChecked();
    m_mrim_accs.value(acc)->sets_current_status = ui.mrim_change_current->isChecked();
    m_mrim_accs.value(acc)->sets_music_status = ui.mrim_change_music_status->isChecked();
    m_mrim_accs.value(acc)->mask_1 = ui.mrim_mask_1->text();
    m_mrim_accs.value(acc)->mask_2 = ui.mrim_mask_2->text();
}

void SettingsUI::initJabberFormFields(const QString& acc){
    ui.jabber_deactivated->setChecked(m_jabber_accs.value(acc)->deactevated);
    ui.jabber_activated->setChecked(!(m_jabber_accs.value(acc)->deactevated));
    ui.jabber_artist->setChecked(m_jabber_accs.value(acc)->artist);
    ui.jabber_title->setChecked(m_jabber_accs.value(acc)->title);
    ui.jabber_album->setChecked(m_jabber_accs.value(acc)->album);
    ui.jabber_track_number->setChecked(m_jabber_accs.value(acc)->number);
    ui.jabber_length->setChecked(m_jabber_accs.value(acc)->length);
    ui.jabber_uri->setChecked(m_jabber_accs.value(acc)->uri);
}

void SettingsUI::saveJabberFormFields(const QString& acc){
    m_jabber_accs.value(acc)->deactevated = ui.jabber_deactivated->isChecked();
    m_jabber_accs.value(acc)->artist = ui.jabber_artist->isChecked();
    m_jabber_accs.value(acc)->title = ui.jabber_title->isChecked();
    m_jabber_accs.value(acc)->album = ui.jabber_album->isChecked();
    m_jabber_accs.value(acc)->number = ui.jabber_track_number->isChecked();
    m_jabber_accs.value(acc)->length = ui.jabber_length->isChecked();
    m_jabber_accs.value(acc)->uri = ui.jabber_uri->isChecked();
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
