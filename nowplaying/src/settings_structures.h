#ifndef SETTINGS_STRUCTURES_H
#define SETTINGS_STRUCTURES_H

#include <QObject>
#include <QString>

class Global : public QObject{
public:
    Global(QObject* parent = 0):QObject(parent){}
    bool is_working;
    bool for_all_accounts;
    QString player;
    QString check_period;
};

struct Oscar : public QObject{
public:
    Oscar(QObject* parent = 0):QObject(parent){}
    bool deactivated;
    bool sets_current_status;
    bool sets_music_status;
    QString mask_1;
    QString mask_2;
};

typedef Oscar MRIM;

struct Jabber : public QObject{
public:
    Jabber(QObject* parent = 0):QObject(parent){}
    bool deactevated;
    bool artist;
    bool title;
    bool album;
    bool number;
    bool length;
    bool uri;
};

#endif // SETTINGS_STRUCTURES_H
