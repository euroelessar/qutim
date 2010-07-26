#ifndef NOWPLAYING_H
#define NOWPLAYING_H

#include "settingsui.h"
#include "settings_structures.h"
#include "player.h"
#include "amarok.h"
#include <qutim/plugin.h>
#include <qutim/extensionicon.h>
#include <qutim/settingslayer.h>
#include <qutim/status.h>
#include <qutim/configbase.h>
#include <qutim/menucontroller.h>
#include <qutim/actiongenerator.h>
#include <qutim/contact.h>
#include <QHash>
#include <QVariantMap>
#include <QDebug>

using namespace qutim_sdk_0_3;

class NowPlaying : public Plugin{
    Q_OBJECT
    Q_CLASSINFO("DebugInfo", "NowPlaying")

public:
    void init();
    bool load();
    bool unload();
private:
    void loadSettings();
    Protocol* m_oscar_protocol;
    Protocol* m_mrim_protocol;
    Protocol* m_jabber_protocol;
    QHash<QString, QString> m_accounts;
    QHash<QString, Oscar*> m_oscar_accs;
    QHash<QString, MRIM*> m_mrim_accs;
    QHash<QString, Jabber*> m_jabber_accs;
    QHash<QString, QString> m_current_status_name;
    bool m_is_working;
    bool m_for_all_accs;
    QString m_player_name;
    quint32 m_update_period;
    Player* m_player;
    void createPlayer();
    void setStatuses(const TrackInfo& info, bool empty = false);
    void setEmptyStatuses();
    void setMRIMStatus(const QString& acc, const QString& text);
    void setOscarStatus(const QString& acc, const QString& text);
    void setJabberStatus(const QString& acc, const TrackInfo& info);
    QString getMessage(const QString& acc, const TrackInfo& info, bool empty = false);
    bool isAccountNeedsInSettingStatus(const QString& acc, bool empty = false);

public slots:
    void configChanged();
    void statusChanged(bool);
    void playingStopped(bool);
    void trackChanged(const TrackInfo&);
    void accountCreated(qutim_sdk_0_3::Account*);
    void statusChanged(qutim_sdk_0_3::Status);
    void accountDeleted();
};

#endif // NOWPLAYING_H
