/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vphotoviewer.h"
#include <qutim/menucontroller.h>
#include <qutim/icon.h>

#include <qutim/declarativeview.h>
#include <qutim/systemintegration.h>
#include <qutim/thememanager.h>
#include <qutim/utils.h>

#include <QDeclarativeContext>
#include <QStringBuilder>

#include <vcontact.h>
#include <vaccount.h>

using namespace qutim_sdk_0_3;

VPhotoViewer::VPhotoViewer(QObject *parent) :
    QObject(parent)
{
    m_contactGen.reset(new ActionGenerator(Icon("camera-photo"),
                                     QT_TRANSLATE_NOOP("Vkontakte", "View photoalbum"),
                                     this,
                                     SLOT(onViewPhotoTriggered(QObject*))));
    m_contactGen->setType(ActionTypeContactList);
    MenuController::addAction<VContact>(m_contactGen.data());
}

VPhotoViewer::~VPhotoViewer()
{
}

void VPhotoViewer::onViewPhotoTriggered(QObject *obj)
{
    VContact *contact = static_cast<VContact*>(obj);
    DeclarativeView *view = new DeclarativeView();
    view->rootContext()->setContextProperty("client", contact->client());
    view->rootContext()->setContextProperty("contact", contact->buddy());
    view->setResizeMode(DeclarativeView::SizeRootObjectToView);

    QString theme_name = "default";
    QString themePath = ThemeManager::path("vphotoalbum",theme_name);
    QString filename =themePath % QLatin1Literal("/main.qml");

    view->setSource(QUrl::fromLocalFile(filename));
    if (!view->status() == DeclarativeView::Ready) {
        qWarning() << "Unable to load qml file";
        view->deleteLater();
    } else {
        centerizeWidget(view);
        SystemIntegration::show(view);
        view->setAttribute(Qt::WA_DeleteOnClose);
    }
}
