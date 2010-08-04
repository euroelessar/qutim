#ifndef NOWPLAYING_H
#define NOWPLAYING_H

#include "settingsui.h"
#include "settings_structures.h"
#include "player.h"
#include <qutim/plugin.h>
#include <QHash>

namespace qutim_sdk_0_3 {
    namespace nowplaying{

        class StopStartActionGenerator;

        class NowPlaying : public Plugin{
            Q_OBJECT
            Q_CLASSINFO("DebugInfo", "NowPlaying")

        public:
            void init();
            bool load();
            bool unload();
        private:
            StopStartActionGenerator* m_stop_start_action;
            void loadSettings();
            Protocol* m_oscar_protocol;
            Protocol* m_mrim_protocol;
            Protocol* m_jabber_protocol;
            QHash<QString, QString> m_accounts;
            QHash<QString, Oscar> m_oscar_accs;
            QHash<QString, MRIM> m_mrim_accs;
            QHash<QString, Jabber> m_jabber_accs;
            QHash<QString, QString> m_current_status_name;
            Global m_global_settings;
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
            void stopStartActionTrigged();
            void configChanged();
            void statusChanged(bool);
            void playingStatusChanged(bool);
            void trackChanged(const TrackInfo&);
            void accountCreated(qutim_sdk_0_3::Account*);
            void statusChanged(qutim_sdk_0_3::Status);
            void accountDeleted();
        };

        class StopStartActionGenerator : public ActionGenerator{
        public:
            StopStartActionGenerator(QObject *module, const QString& text);
            void showImpl(QAction *action, QObject *obj);
            void setText(const QString& text){m_text = text;}
            QString m_text;
        };
    }
}
#endif // NOWPLAYING_H
