/**************************************************************************
*  Rghost Uploader. qutIM 0.3 plugin, that allows users to send files via *
*  rghost.ru serviсe.                                                     *
*  Copyright (C) 2010  Ian Kazlauskas <nayzak90@googlemail.com>           *
*                                                                         *
*  This library is free software: you can redistribute it and/or modify   *
*  it under the terms of the GNU General Public License as published by   *
*  the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                    *
*                                                                         *
*  This library is distributed in the hope that it will be useful,        *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*  GNU General Public License for more details.                           *
*                                                                         *
*  You should have received a copy of the GNU General Public License      *
*  along with this library.  If not, see <http://www.gnu.org/licenses/>.  *
***************************************************************************/

#ifndef RGHOSTUPLOADERPLUGIN_H
#define RGHOSTUPLOADERPLUGIN_H

#include <qutim/plugin.h>
#include <qutim/menucontroller.h>
#include <datauploader.h>
#include <qutim/contact.h>
#include <qutim/message.h>
#include <qutim/account.h>
#include <qutim/messagesession.h>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QHash>
#include <QNetworkAccessManager>

using namespace qutim_sdk_0_3;

class RghostUploaderPlugin : public Plugin{

Q_OBJECT
Q_CLASSINFO("DebugInfo", "rghost uploader")

public:
    virtual void init();
    virtual bool load();
    virtual bool unload();

private:
    QHash<DataUploader*, Contact* > m_uploads;
    QNetworkAccessManager* m_transfer_manager;
private slots:
    void uploadMenuClicked(QObject*);
    void fileUploaded(QString message);
};

#endif // RGHOSTUPLOADERPLUGIN_H
