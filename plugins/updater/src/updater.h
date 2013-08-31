/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef UPDATERPLUGIN_H
#define UPDATERPLUGIN_H

#include <qutim/plugin.h>
#include <QNetworkAccessManager>
#include <QFutureWatcher>
#include <QBasicTimer>
#include <QQueue>
#include <QUrl>

namespace qutim_sdk_0_3
{
	class SettingsItem;
}

namespace Updater {

class UpdaterPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("DebugName", "Updater")
	Q_CLASSINFO("Uses", "ChatLayer")

public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
	
	void timerEvent(QTimerEvent *);
	void updateIcons();
	
private slots:
	void onReplyFinished(QNetworkReply *reply);
	void onCheckFinished();
	void requestNextUrl();

private:
	struct FileInfo
	{
		typedef QList<FileInfo> List;
		bool isInvalid() const;
		
		QByteArray sha1;
		QByteArray md5;
		QString filePath;
		QString fileName;
	};
	
	FileInfo::List checkList(const FileInfo::List &original);

	QBasicTimer m_timer;
	QScopedPointer<QFutureWatcher<FileInfo> > m_watcher;
	QScopedPointer<QNetworkAccessManager> m_manager;
	QQueue<QPair<QUrl, QString> > m_queue;
};
}

#endif // UPDATERPLUGIN_H

