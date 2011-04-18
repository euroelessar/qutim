/****************************************************************************
 *  abstractcontactmodel.cpp
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

#include "abstractcontactmodel_p.h"
#include <qutim/metacontactmanager.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>

namespace Core {
namespace SimpleContactList {

using namespace qutim_sdk_0_3;

AbstractContactModel::AbstractContactModel(AbstractContactModelPrivate *d, QObject *parent) :
	QAbstractItemModel(parent), d_ptr(d)
{
}

AbstractContactModel::~AbstractContactModel()
{

}

bool AbstractContactModel::showOffline() const
{
	return true;
}

void AbstractContactModel::hideShowOffline()
{

}

void AbstractContactModel::filterList(const QString &)
{

}

void AbstractContactModel::filterList(const QStringList &)
{

}

QStringList AbstractContactModel::selectedTags() const
{
	return QStringList();
}

QStringList AbstractContactModel::tags() const
{
	return QStringList();
}

} // namespace SimpleContactList
} // namespace Core
