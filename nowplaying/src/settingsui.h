#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include "ui_settings.h"
#include "settings_structures.h"
#include <qutim/settingswidget.h>
#include <qutim/configbase.h>
#include <qutim/libqutim_global.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <QHash>
#include <QDebug>

namespace qutim_sdk_0_3 {
namespace nowplaying{

class SettingsUI : public SettingsWidget{

Q_OBJECT

public:
        SettingsUI();
        ~SettingsUI();
        virtual void loadImpl();
        virtual void saveImpl();
        virtual void cancelImpl();
private:
        Ui::settingsUi ui;
        QHash<QString, QString> m_accounts;
        QHash<QString, Oscar> m_oscar_accs;
        QHash<QString, MRIM> m_mrim_accs;
        QHash<QString, Jabber> m_jabber_accs;
        bool m_is_working;
        QString m_current_account;
        void initFormValues();
        void initOscarFormFields(const QString& acc);
        void initMRIMFormFields(const QString& acc);
        void initJabberFormFields(const QString& acc);
        void saveOscarFormFields(const QString& acc);
        void saveMRIMFormFields(const QString& acc);
        void saveJabberFormFields(const QString& acc);
        void initAccountFormField(const QString& acc);
        void saveAccountFormField(const QString& acc);
signals:
        void configSaved();
        void statusChanged(bool);
public slots:
        void accountChanged(QString);
        void stopButtonClicked();
        void forAllAccsClicked();
};

}}
#endif





