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

#ifndef PLUGPACKAGEMODEL_H
#define PLUGPACKAGEMODEL_H

#include <QAbstractItemModel>
#include <QAbstractItemView>

class PlugPackageModel : public QAbstractItemModel
{
Q_OBJECT
public:
	PlugPackageModel (QObject *parent = 0, QAbstractItemView *packageView = 0);
// 	PlugPackageModel (const QList<Package*> &packages, QObject *parent = 0, QAbstractItemView *packageView = 0);
	
};

#endif // PLUGPACKAGEMODEL_H
