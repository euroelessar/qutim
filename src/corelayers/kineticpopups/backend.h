/****************************************************************************
 *  backend.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef BACKEND_H
#define BACKEND_H

#include <qutim/notificationslayer.h>

using namespace qutim_sdk_0_3;

namespace KineticPopups
{
	class Popup;
	class Backend : public PopupBackend
	{
		Q_OBJECT
	public:
		virtual void show(Notifications::Type type, QObject* sender,
						  const QString& body, const QVariant& data);
		Backend ();
	public slots:
		void updateSettings();
	private:
		int m_id_counter;
	};

}

#endif //BACKEND_H
