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
#ifndef VPHOTOVIEW_H
#define VPHOTOVIEW_H

#include <QWidget>
#include <QtDeclarative/QDeclarativeView>
#include <QVariantList>

class VContact;
class VConnection;
class QAbstractItemModel;
namespace Vkontakte
{

class VReply : public QObject
{
};

class VPhotoView : public QDeclarativeView
{
    Q_OBJECT
//	Q_PROPERTY (QObjectList albums READ albums NOTIFY albumsChanged)
public:
	explicit VPhotoView(QObject *contact);
	QObjectList albums() const;
signals:
	void albumsChanged();
public slots:
	void updateAlbums();
private slots:
	void onUpdateAlbumsFinished();
private:
	QObject *m_owner;
	QString m_uid;
	QObjectList m_albums;
	VConnection *m_connection;
	QMap<QString,QAbstractItemModel*> m_models;
};

}
#endif // VPHOTOVIEW_H

