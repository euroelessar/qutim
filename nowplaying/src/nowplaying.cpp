#include "nowplaying.h"
#include "qutim/event.h"
#include "QTest"

void NowPlaying::init(){
        setInfo(QT_TRANSLATE_NOOP("Plugin", "Now Playing"),
                        QT_TRANSLATE_NOOP("Plugin", "Now playing plugin"),
                        PLUGIN_VERSION(0, 1, 0, 0));
        addAuthor(QT_TRANSLATE_NOOP("Author","Kazlauskas Ian"),
                          QT_TRANSLATE_NOOP("Task","Developer"),
                          QLatin1String("nayzak90@googlemail.com"));
}

bool NowPlaying::load(){
    SettingsItem* settings = new GeneralSettingsItem<SettingsUI>(
                        Settings::Plugin,
                        QIcon(":images/images/logo.png"),
                        QT_TRANSLATE_NOOP("NowPlaying", "Now Playing"));
    connect (((SettingsUI*)settings->widget()), SIGNAL(configSaved()), this, SLOT(configChanged()));
    connect (((SettingsUI*)settings->widget()), SIGNAL(statusChanged(bool)), this, SLOT(statusChanged(bool)));
    Settings::registerItem(settings);
    loadSettings();
    createPlayer();
    if (m_is_working){
        m_player->startWatching();
    }
    connect(m_oscar_protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(accountCreated(qutim_sdk_0_3::Account*)));
    connect(m_jabber_protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(accountCreated(qutim_sdk_0_3::Account*)));
    connect(m_mrim_protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(accountCreated(qutim_sdk_0_3::Account*)));
    return true;
}

bool NowPlaying::unload(){
    setEmptyStatuses();
    return true;
}

void NowPlaying::loadSettings(){
    QString proto_id;
    QString acc_id;
    m_accounts.insert("oscar", "icq");
    m_oscar_accs.insert("oscar", new Oscar(this));
    m_accounts.insert("mrim", "mrim");
    m_mrim_accs.insert("mrim", new MRIM(this));
    m_accounts.insert("jabber", "jabber");
    m_jabber_accs.insert("jabber", new Jabber(this));
    foreach (Protocol* proto, allProtocols()) {
        if (proto->id() == "icq"){
            m_oscar_protocol = proto;
        } else if (proto->id() == "mrim"){
            m_mrim_protocol = proto;
        } else if (proto->id() == "jabber"){
            m_jabber_protocol = proto;
        }
        foreach(Account* acc, proto->accounts()){
            proto_id = proto->id();
            acc_id = acc->id();
            m_accounts.insert(acc_id, proto_id);
            if (proto_id == "icq"){
                m_oscar_accs.insert(acc_id, new Oscar(this));
                connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status)), this, SLOT(statusChanged(qutim_sdk_0_3::Status)));
                connect(acc, SIGNAL(destroyed()), this, SLOT(accountDeleted()));
            } else if (proto_id == "mrim"){
                m_mrim_accs.insert(acc_id, new MRIM(this));
                connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status)), this, SLOT(statusChanged(qutim_sdk_0_3::Status)));
                connect(acc, SIGNAL(destroyed()), this, SLOT(accountDeleted()));
            } else if (proto_id == "jabber"){
                m_jabber_accs.insert(acc_id, new Jabber(this));
                connect(acc, SIGNAL(destroyed()), this, SLOT(accountDeleted()));
            }
        }
    }
    Config group = Config().group("nowplaying");
    Config global = group.group("global");
    m_is_working = global.value("is_working", false);
    m_player_name = global.value("player", QString("Amarok"));
    m_update_period = global.value("check_period", (quint32) 1);
    m_update_period *= 1000;
    m_for_all_accs = global.value("for_all_accounts", true);

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
}

void NowPlaying::configChanged(){
    m_player->stopWatching();
    setEmptyStatuses();
    QString player = m_player_name;
    quint32 update_period = m_update_period;
    loadSettings();
    if ((player != m_player_name) || (update_period != m_update_period)){
        delete m_player;
        createPlayer();
        if (m_is_working){
            m_player->startWatching();
            setStatuses(m_player->getTrackInfo());
        }
    }
    if (!m_is_working){
        m_player->stopWatching();
    } else{
        m_player->startWatching();
        setStatuses(m_player->getTrackInfo());
    }
}

void NowPlaying::createPlayer(){
    m_player = NULL;
    if (m_player_name == "Amarok"){
        m_player = new Amarok(m_update_period, this);
    }
    if (m_player != NULL){
        connect(m_player, SIGNAL(playingStopped(bool)), this, SLOT(playingStopped(bool)));
        connect(m_player, SIGNAL(trackChanged(const TrackInfo&)), this, SLOT(trackChanged(const TrackInfo&)));
    }
}

void NowPlaying::statusChanged(bool is_working){
    qDebug()<<"!!!! Working status changed";
    if (is_working){
        m_player->startWatching();
        m_is_working = is_working;
        setStatuses(m_player->getTrackInfo());
    } else{
        m_player->stopWatching();
        setEmptyStatuses();
        m_is_working = is_working;
    }
}

void NowPlaying::playingStopped(bool stopped){
    if(stopped){
        setEmptyStatuses();
    } else{
        setStatuses(m_player->getTrackInfo());
    }
}

void NowPlaying::trackChanged(const TrackInfo& info){
    setStatuses(info);
}

void NowPlaying::setStatuses(const TrackInfo& info, bool empty){
    foreach (QString acc, m_oscar_accs.keys()){
        if (acc != "oscar"){
            if (isAccountNeedsInSettingStatus(acc, empty)){
                QString message = (empty) ? QString("") : getMessage(acc, info);
                setOscarStatus(acc, message);
            }
        }
    }
    foreach (QString acc, m_jabber_accs.keys()){
        if (acc != "jabber"){
            if (isAccountNeedsInSettingStatus(acc))
                setJabberStatus(acc, info);
        }
    }
}

void NowPlaying::setEmptyStatuses(){
    setStatuses(TrackInfo(), true);
}

void NowPlaying::setMRIMStatus(const QString& acc, const QString& message){
    Q_UNUSED(acc);
    Q_UNUSED(message);
}

void NowPlaying::setOscarStatus(const QString& acc, const QString& message){
    qDebug()<<"!!!!!!!! set status for"<<acc;
    Status status = m_oscar_protocol->account(acc)->status();
    QVariantMap extStatus;
    QVariantMap cur_status = status.extendedInfo("xstatus");
    extStatus.insert("id", cur_status.value("id"));
    extStatus.insert("title", cur_status.value("title"));
    extStatus.insert("icon", cur_status.value("icon"));
    extStatus.insert("name", cur_status.value("name"));
    extStatus.insert("desc", message);
    status.setExtendedInfo("xstatus", extStatus);
    m_oscar_protocol->account(acc)->setStatus(status);
    m_current_status_name.insert(acc, cur_status.value("name").toString());
    qDebug()<<"!!! Status: "<<message<<"!!!";
}

void NowPlaying::setJabberStatus(const QString& acc, const TrackInfo& info){
    qDebug()<<"!!!!!!!! set status for"<<acc;
    QVariantMap tune;
    Jabber* conf = (m_for_all_accs) ? m_jabber_accs.value("jabber") : m_jabber_accs.value(acc);
    if (conf->album)
        tune.insert("source", info.album);
    if (conf->artist)
        tune.insert("artist", info.artist);
    if (conf->length)
        tune.insert("length", info.time);
    if (conf->number)
        tune.insert("track", info.track_number);
    if (conf->title)
        tune.insert("title", info.title);
    if (conf->uri){
        tune.insert("uri", info.uri);
    }
    Event ev("jabber-personal-event", "tune", tune, true);
    qApp->sendEvent(m_jabber_protocol->account(acc), &ev);
}

void NowPlaying::accountCreated(qutim_sdk_0_3::Account* acc){
    qDebug()<<"!!!!!!!"<<acc->id()<<"created";
    QString proto_id = acc->protocol()->id();
    QString acc_id = acc->id();
    m_accounts.insert(acc_id, proto_id);
    Config group = Config().group("nowplaying");
    if (proto_id == "icq"){
        Oscar* conf = new Oscar(this);
        Config oscar = group.group(acc_id);
        conf->deactivated = oscar.value("deactivated", false);
        conf->sets_current_status = oscar.value("sets_current_status", false);
        conf->sets_music_status = oscar.value("sets_music_status", true);
        conf->mask_1 = oscar.value("mask_1", QString("Now playing: %artist - %title"));
        conf->mask_2 = oscar.value("mask_2", QString("%artist - %title"));
        m_oscar_accs.insert(acc_id, conf);
        if (isAccountNeedsInSettingStatus(acc_id))
            setOscarStatus(acc_id, getMessage(acc_id, m_player->getTrackInfo()));
        m_current_status_name.insert(acc_id, acc->status().extendedInfo("xstatus").value("name").toString());
        connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status)), this, SLOT(statusChanged(qutim_sdk_0_3::Status)));
        connect(acc, SIGNAL(destroyed()), this, SLOT(accountDeleted()));
    } else if (proto_id == "mrim"){
        MRIM* conf = new MRIM(this);
        Config mrim = group.group(acc_id);
        conf->deactivated = mrim.value("deactivated", false);
        conf->sets_current_status = mrim.value("sets_current_status", false);
        conf->sets_music_status = mrim.value("sets_music_status", true);
        conf->mask_1 = mrim.value("mask_1", QString("Now playing: %artist - %title"));
        conf->mask_2 = mrim.value("mask_2", QString("%artist - %title"));
        m_mrim_accs.insert(acc_id, conf);
        if (isAccountNeedsInSettingStatus(acc_id))
            setMRIMStatus(acc_id, getMessage(acc_id, m_player->getTrackInfo()));
        connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status)), this, SLOT(statusChanged(qutim_sdk_0_3::Status)));
        connect(acc, SIGNAL(destroyed()), this, SLOT(accountDeleted()));
    } else if (proto_id == "jabber"){
        Jabber* conf = new Jabber(this);
        Config jabber = group.group(acc_id);
        conf->deactevated = jabber.value("deactevated", false);
        conf->artist = jabber.value("artist", true);
        conf->title = jabber.value("title", true);
        conf->album = jabber.value("album", false);
        conf->length = jabber.value("length", false);
        conf->number = jabber.value("number", false);
        conf->uri = jabber.value("uri", false);
        m_jabber_accs.insert(acc_id, conf);
        if (isAccountNeedsInSettingStatus(acc_id))
            setJabberStatus(acc_id, m_player->getTrackInfo());
        connect(acc, SIGNAL(destroyed()), this, SLOT(accountDeleted()));
    }
}

void NowPlaying::accountDeleted(){
    Account* acc = qobject_cast<Account*>(sender());
    QString proto_id = acc->protocol()->id();
    QString acc_id = acc->id();
    m_accounts.remove(acc->id());
    if (proto_id == "icq"){
        Oscar* config = m_oscar_accs.value(acc_id);
        m_oscar_accs.remove(acc_id);
        delete config;
    } else if (proto_id == "mrim"){
        MRIM* config = m_mrim_accs.value(acc_id);
        m_mrim_accs.remove(acc_id);
        delete config;
    } else if (proto_id == "jabber"){
        Jabber* config = m_jabber_accs.value(acc_id);
        m_jabber_accs.remove(acc_id);
        delete config;
    }
}

QString NowPlaying::getMessage(const QString& acc, const TrackInfo& info, bool empty){
    if (empty) return QString();
    QString proto_id = m_accounts.value(acc);
    if (proto_id == "icq"){
        Oscar* config = (m_for_all_accs) ? m_oscar_accs.value("oscar") : m_oscar_accs.value(acc);
        QString message = (config->sets_current_status) ? config->mask_1 : config->mask_2;
        message.replace("%artist", info.artist).replace("%title", info.title).
                replace("%album", info.album).replace("%track", info.track_number).
                replace("%file", info.file_name).replace("%uri", info.uri).
                replace("%time", info.time);
        return message;
    } else if (proto_id == "mrim"){
        MRIM* config = (m_for_all_accs) ? m_mrim_accs.value("mrim") : m_mrim_accs.value(acc);
        QString message = (config->sets_current_status) ? config->mask_1 : config->mask_2;
        message.replace("%artist", info.artist).replace("%title", info.title).
                replace("%album", info.album).replace("%track", info.track_number).
                replace("%file", info.file_name).replace("%uri", info.uri).
                replace("%time", info.time);
        return message;
    } else {
        return QString();
    }
}

void NowPlaying::statusChanged(qutim_sdk_0_3::Status status){
    if (m_is_working){
        Account* account = qobject_cast<Account*>(sender());
        QString acc = account->id();
        if (m_current_status_name.contains(acc)){
            if (m_accounts.value(acc) == "icq"){
                QString name = status.extendedInfo("xstatus").value("name").toString();
                QString cur_name = m_current_status_name.value(acc);
                if (name != cur_name){
                    qDebug()<<"!!!!!!!!!!!!!!!! x status changed by user";
                    Config config = account->config("xstatus");
                    config.beginGroup(cur_name);
                    Oscar* conf = (m_for_all_accs) ? m_oscar_accs.value("oscar") : m_oscar_accs.value(acc);
                    if (!conf->deactivated){
                        if (conf->sets_current_status){
                            config.setValue("message", "");
                        }
                    }
                    config.endGroup();
                    config.sync();
                    m_current_status_name.insert(acc, name);
                }
            }
        }
    }
}

bool NowPlaying::isAccountNeedsInSettingStatus(const QString& acc, bool empty){
    if (m_is_working){
        QString proto_id = m_accounts.value(acc);
        if (proto_id == "icq"){
            Oscar* config = (m_for_all_accs) ? m_oscar_accs.value("oscar") : m_oscar_accs.value(acc);
            if (!config->deactivated){
                QVariantMap xstatus = m_oscar_protocol->account(acc)->status().extendedInfo("xstatus");
                if (config->sets_current_status &&
                    xstatus.value("desc") != getMessage(acc, m_player->getTrackInfo(), empty)){
                    return true;
                } else if (config->sets_music_status &&
                           xstatus.value("name") == QVariant("music") &&
                           xstatus.value("desc") != getMessage(acc, m_player->getTrackInfo(), empty)){
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (proto_id == "jabber"){
            Jabber* config = (m_for_all_accs) ? m_jabber_accs.value("jabber") : m_jabber_accs.value(acc);
            return !config->deactevated;
        } else if (proto_id == "mrim"){
            MRIM* config = (m_for_all_accs) ? m_mrim_accs.value("mrim") : m_mrim_accs.value(acc);
            if (!config->deactivated){
                if (config->sets_current_status){
                    //complete!!!
                    return true;
                } else if (config->sets_music_status){
                    //complete!!!
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    } else {
        return false;
    }
    return false;
}

QUTIM_EXPORT_PLUGIN(NowPlaying)
