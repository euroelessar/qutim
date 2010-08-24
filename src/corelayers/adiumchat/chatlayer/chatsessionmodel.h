/****************************************************************************
 *  chatsessionmodel.h
 *
 *   Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef CHATSESSIONMODEL_H
#define CHATSESSIONMODEL_H

#include <QAbstractListModel>
#include "chatsessionimpl.h"

namespace Core
{
	namespace AdiumChat
	{
		using namespace qutim_sdk_0_3;

		class ChatSessionModel : public QAbstractListModel
		{
			Q_OBJECT
		public:
			explicit ChatSessionModel(ChatSessionImpl *parent = 0);
			virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
			virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
			void addContact(qutim_sdk_0_3::Buddy *c);
			void removeContact(qutim_sdk_0_3::Buddy *c);
		private slots:
			void onNameChanged(const QString &name);
			void onStatusChanged(const qutim_sdk_0_3::Status &status);
		private:
			QList<Buddy*> m_units;
		};
	}
}
#endif // CHATSESSIONMODEL_H
