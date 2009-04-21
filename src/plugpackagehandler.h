/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PLUGPACKAGEHANDLER_H
#define PLUGPACKAGEHANDLER_H

#include <QObject>
#include "plugxmlhandler.h"
#include "plugpackagemodel.h"

class plugPackageHandler : public QObject
{
Q_OBJECT
public:
	plugPackageHandler(plugPackageModel * plug_package_model,QObject *parent = 0);
	~plugPackageHandler();
	void getPackageList();
	void setPlugPackageModel (plugPackageModel * plug_package_model);
private:
	plugPackageModel *m_plug_package_model;
	void updatePlugPackageModel (const QHash<quint16, packageInfo> &package_list);
};

#endif // PLUGPACKAGEHANDLER_H
