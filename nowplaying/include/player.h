#ifndef PLAYER_H
#define PLAYER_H

#include "trackinfo.h"
#include <QObject>

namespace qutim_sdk_0_3 {
namespace nowplaying{

class Player : public QObject {
    Q_OBJECT
public:

    virtual ~Player(){}
    virtual QString playerName() = 0;
    virtual TrackInfo trackInfo() = 0;
    virtual void init() = 0;
    /* isPlaying()
     * must return true if track is playing
     * if paused or stopped return false
     */
    virtual bool isPlaying() = 0;

    /* startWatching()
     * when call this method Player object
     * must start scan player state
     */
    virtual void startWatching() = 0;

    /* stopWatching()
     * when call this method Player object
     * must stop scan player state
     */
    virtual void stopWatching() = 0;

signals:

    /* playingStatusChanged(bool)
     * Emits false, if track have stopped or paused
     * Emits true, if track continued playing
     * after pause or stop
     */
    void playingStatusChanged(bool is_playing);
    void trackChanged(const TrackInfo&);
};
}
}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::nowplaying::Player, "org.qutim.qutim_sdk_0_3.nowplaying.Player")
#endif // PLAYER_H
