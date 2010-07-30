/****************************************************************************
 *  contactsmodel.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "contactsmodel.h"
#include "3rdparty/itemdelegate/itemdelegate.h"

namespace Core {

void ContactsModel::setRequest(const RequestPtr &request)
{
	beginResetModel();
	if (m_request)
		m_request->disconnect(this);
	m_request = request;
	if (m_request) {
		connect(m_request.data(), SIGNAL(contactAboutToBeAdded(int)), SLOT(contactAboutToBeAdded(int)));
		connect(m_request.data(), SIGNAL(contactAdded(int)), SLOT(contactAdded(int)));
	}
	endResetModel();
}

int ContactsModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	if (m_request)
		return m_request->contactCount();
	else
		return 0;
}

int ContactsModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
	if (!m_request)
		return QVariant();
	if (role == DescriptionRole) {
		int row = index.row();
		QVariantMap fields;
		for (int i = 1, c = m_request->columnCount(); i < c; ++i) {
			QVariant data = m_request->data(row, i, Qt::DisplayRole);
			if (data.isNull())
				continue;
			if (data.canConvert(QVariant::String) && data.toString().isEmpty())
				continue;
			fields.insert(m_request->headerData(i, Qt::DisplayRole).toString(), data);
		}
		return fields;
	} else if (index.column() == 0) {
		return m_request->data(index.row(), 0, role);
	}
	return QVariant();
}

QVariant ContactsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant();
}

void ContactsModel::contactAboutToBeAdded(int row)
{
	beginInsertRows(QModelIndex(), row, row);
}

void ContactsModel::contactAdded(int row)
{
	Q_UNUSED(row);
	endInsertRows();
}


} // namespace Core
