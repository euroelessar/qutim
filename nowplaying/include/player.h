#ifndef PLAYER_H
#define PLAYER_H

#include "trackinfo.h"
#include <qutim/event.h>
#include <QObject>

namespace qutim_sdk_0_3 {

namespace nowplaying 
{
	class StateEvent : public Event
	{
	public:
		inline StateEvent(bool isPlaying) : Event(eventId(), isPlaying) {}
		inline ~StateEvent() {}
		
		static quint16 eventId() { return registerType("now-playing-state"); }
		
		bool isPlaying() { return args[0].toBool(); }
	};
	
	class TrackInfoEvent : public Event
	{
	public:
		inline TrackInfoEvent(const TrackInfo &info) : Event(eventId(), qVariantFromValue(info)) {}
		inline ~TrackInfoEvent() {}
		
		static quint16 eventId() { return registerType("now-playing-track"); }
		
		TrackInfo trackInfo() { return args[0].value<TrackInfo>(); }
	};
	
	class PlayerEvent : public Event
	{
	public:
		enum Info
		{
			Available,
			Unavailable
		};

		inline PlayerEvent(const QString &id, Info i) : Event(eventId(), id, int(i)) {}
		inline ~PlayerEvent() {}
		
		static quint16 eventId() { return registerType("now-playing-player"); }
		
		QString playerId() { return args[0].toString(); }
		Info playerInfo() { return static_cast<Info>(args[1].toInt()); }
	};

	class Player
	{
	public:
		virtual ~Player() {}
		
		virtual QString id() = 0;
		
		virtual void init() = 0;
		
		virtual void requestState() = 0;
		virtual void requestTrackInfo() = 0;

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
	};
	
	class PlayerFactory
	{
	public:
		virtual ~PlayerFactory() {}
		
		/* players()
		 * returnes map with PlayerId as keys
		 * and PlayerName as values
		 */
		virtual QMap<QString,QString> players() = 0;
		
		virtual QObject *player(const QString &id) = 0;
	};
} }

Q_DECLARE_INTERFACE(qutim_sdk_0_3::nowplaying::Player, "org.qutim.qutim_sdk_0_3.nowplaying.Player")
Q_DECLARE_INTERFACE(qutim_sdk_0_3::nowplaying::PlayerFactory, "org.qutim.qutim_sdk_0_3.nowplaying.PlayerFactory")
#endif // PLAYER_H
