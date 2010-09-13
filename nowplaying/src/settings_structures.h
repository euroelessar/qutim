#ifndef SETTINGS_STRUCTURES_H
#define SETTINGS_STRUCTURES_H

#include <QString>

namespace qutim_sdk_0_3 {
namespace nowplaying{

struct Global{
    bool is_working;
    bool for_all_accs;
    QString player;
};

struct Oscar{
    bool deactivated;
    bool sets_current_status;
    bool sets_music_status;
    QString mask_1;
    QString mask_2;
};

typedef Oscar MRIM;

struct Jabber{
    bool deactevated;
    bool artist;
    bool title;
    bool album;
    bool number;
    bool length;
    bool uri;
};

}}

#endif // SETTINGS_STRUCTURES_H
