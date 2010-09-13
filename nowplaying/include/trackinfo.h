#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QString>

namespace qutim_sdk_0_3 {
namespace nowplaying{

struct TrackInfo{
    QString artist;
    QString title;
    QString album;
    QString time;
    QString track_number;
    QString file_name;
    QString uri;
};

}}
#endif // TRACKINFO_H
