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

#include "rghostuploaderplugin.h"

void RghostUploaderPlugin::init(){
    setInfo(QT_TRANSLATE_NOOP("Plugin", "rghost Uploader"),
                        QT_TRANSLATE_NOOP("Plugin", "Upload files to rghost.ru service"),
                        PLUGIN_VERSION(0, 1, 0, 0));
    addAuthor(QT_TRANSLATE_NOOP("Author","Ian Kaslauskas"),
                          QT_TRANSLATE_NOOP("Task","Developer"),
                          QLatin1String("nayzak90@googlemail.com"));
}

bool RghostUploaderPlugin::load(){
    m_transfer_manager = new QNetworkAccessManager(this);
    MenuController::addAction<Contact>(new ActionGenerator(QIcon(":images/images/logo.ico"),QT_TRANSLATE_NOOP("rghost", "Send file via rghost.ru"),this, SLOT(uploadMenuClicked())));
    return true;
}

bool RghostUploaderPlugin::unload(){
    return false;
}

void RghostUploaderPlugin::uploadMenuClicked(){
    QAction* action = qobject_cast<QAction*>(sender());
    Contact* contact = qobject_cast<Contact*>(action->data().value<MenuController*>());
    QString file_name = QFileDialog::getOpenFileName(0, tr("Choose file"));
    if (file_name.isEmpty()){
        return;
    }
    DataUploader* uploader = new DataUploader(file_name, m_transfer_manager);
    m_uploads.insert(uploader, contact);
    uploader->uploadFile();
    connect(uploader, SIGNAL(processFinished(QString)), this, SLOT(fileUploaded(QString)));
}

void RghostUploaderPlugin::fileUploaded(QString message){
    DataUploader* uploader = qobject_cast<DataUploader*>(sender());
    Contact* contact = m_uploads.value(uploader, NULL);
    m_uploads.remove(uploader);
    if (message != "!!!ERROR!!!"){
        Message mes(message);
        ChatSession *session = ChatLayer::instance()->get(contact);
        mes.setChatUnit(session->getUnit());
        session->appendMessage(mes);
    }
    delete uploader;
    uploader = NULL;
}

QUTIM_EXPORT_PLUGIN(RghostUploaderPlugin)
