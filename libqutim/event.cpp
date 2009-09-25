/****************************************************************************
 *  event.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "event.h"
#include <QtCore/QPointer>
#include <QtCore/QDebug>
#include <QtCore/QSharedData>

namespace qutim_sdk_0_3
{
	// TODO: May be another namespace?..
	class EventTrack : public QSharedData
	{
	public:
		typedef QExplicitlySharedDataPointer<EventTrack> Ptr;
		inline EventTrack(QObject *o, EventManager::Method m)
				: object(o), method(m) {}
		QPointer<QObject> object;
		EventManager::Method method;
	};
	static QVector<QByteArray> event_ids;
	typedef QPair<quint16, EventTrack::Ptr> EventHandlerInfo;
	static QVector<QVector<EventHandlerInfo> > event_tracks;


	quint16 EventManager::eventId(const char *id)
	{
		if(!id)
			return 0xffff;
		for(int i = 0; i < event_ids.size(); i++)
			if(event_ids[i] == id)
				return i;
		event_ids.append(id);
		event_tracks.resize(event_ids.size());
		return event_ids.size() - 1;
	}

	const char *EventManager::eventId(quint16 id)
	{
		if(id >= event_ids.size())
			return 0;
		return event_ids[id];
	}

	quint16 EventManager::send(Event &event)
	{
		if(event.id >= event_ids.size())
			return 0;
		QVector<EventHandlerInfo> &infos = event_tracks[event.id];
		for(int i = 0; i < infos.size(); i++)
		{
			if(infos[i].second->object.isNull())
			{
				infos.remove(i);
				i--;
			}
			else
			{
				EventTrack::Ptr &track = infos[i].second;
				(track->object->*track->method)(event);
			}
		}
		return infos.size();
	}

	quint16 EventManager::register_helper(QObject *obj, Method method, const char *text_id, quint16 priority)
	{
		Q_ASSERT_X(obj, "EventManager::register_helper", "Object is invalid");
		quint16 id = eventId(text_id);
		if(id == 0xffff)
			return id;
		EventHandlerInfo info = qMakePair(priority, EventTrack::Ptr(new EventTrack(obj, method)));
		int position = qUpperBound(event_tracks[id], info) - event_tracks[id].constBegin();
		event_tracks[id].insert(position, info);
		return id;
	}

	QDebug operator<<(QDebug dbg, const Event &e)
	{
		QVariantList list;
		for(int i = 0; i < e.args.size() && !e.args[i].isNull(); i++)
			list << e.args[i];
		dbg.nospace() << "(\"" << EventManager::eventId(e.id) << "\": ";
		if(!e.args[0].isNull())
			dbg << e.args[0];
		for(int i = 1; i < e.args.size() && !e.args[i].isNull(); i++)
			dbg << ", " << e.args[i];
		return dbg << " ) ";
	}
}
