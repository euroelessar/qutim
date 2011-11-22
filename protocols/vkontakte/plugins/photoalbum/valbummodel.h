/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#ifndef VALBUMMODEL_H
#define VALBUMMODEL_H
#include <QStandardItemModel>

class VConnection;
namespace Vkontakte
{

//class VAlbumModel : public QStandardItemModel
//{
//	Q_OBJECT
//public:
//	enum AlbumRole
//	{
//		SmallImageRole,
//		BigImageRole
//	};
//	VAlbumModel(VConnection *c,const QString &uid,const QString &aid);
//	Q_INVOKABLE void update();
//private slots:
//	void onUpdateFinished();
//private:
//	VConnection *m_connection;
//	QString m_aid;
//	QString m_uid;
//};

class VAlbum : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QVariantList photos READ photos NOTIFY photosChanged)
public:
	VAlbum(VConnection *c,const QString &uid,const QString &aid);
	void setTitle(const QString &string);
	QString title() const;
	QVariantList photos() const;
public slots:
	void update();
signals:
	void titleChanged();
	void photosChanged();
private slots:
	void onUpdateFinished();
private:
	VConnection *m_connection;
	QString m_aid;
	QString m_uid;
	QVariantList m_photos;
	QString m_title;
};

}

#endif // VALBUMMODEL_H

