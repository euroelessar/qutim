/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2013 Roman Tretyakov <roman@trett.ru>
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

#ifndef ADIUMCHATVIEWWIDGET_H
#define ADIUMCHATVIEWWIDGET_H

#include <QWebView>
#include <qutim/adiumchat/chatviewfactory.h>
#include <qutim/declarativeview.h>
#include <QPointer>
#include "yandexdomaindefine.h"

class QDeclarativeItem;

namespace Adium {

class WebViewController;
class WebViewWidget : public QWebView, public Core::AdiumChat::ChatViewWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewWidget)
public:
	WebViewWidget(QWidget *parent = 0);
    virtual void setViewController(QObject* controller);
private:
	QPointer<WebViewController> m_controller;
	QString m_searcher;
	YandexDomainDefine m_yandexDomainDefine;
	QUrl m_urlForOpen;
#ifdef Q_WS_MAEMO_5
	bool mousePressed;
	virtual bool eventFilter(QObject *, QEvent *e);
#endif
public slots:
	void showCustomContextMenu(const QPoint & point);
	void insertQuoteText();
	void searchSelectedText();
	void openLinkFromContextMenu();
	void setPrevFocus(QObject *);
	void openChat();
	void openConference();
};

} // namespace Adium

#endif // ADIUMCHATVIEWWIDGET_H

