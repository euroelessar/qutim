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

#ifndef PLUGPACKAGECACHE_H
#define PLUGPACKAGECACHE_H

#include <QObject>
#include <QProgressBar>
#include "mirror.h"
#include "plugdownloader.h"

class plugPackageCache : public QObject
{
Q_OBJECT
public:
	plugPackageCache(QProgressBar *progress_bar,QObject *parent);
	~plugPackageCache();
	void updatePackagesCache (); //!update packages cache file
private:
	void readMirrorList();
	QList<mirrorInfo> mirror_list;
	QString platform; //TODO спросить у Элесара, как её выдернуть из автоопределия платформы Кутима
	QProgressBar *m_progress_bar;
	
private slots:
	void onDownloadFinished(const QString&);
};

#endif // PLUGPACKAGECACHE_H
