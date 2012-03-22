/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef QUICKCHATVIEWCONTROLLER_H
#define QUICKCHATVIEWCONTROLLER_H

#include "../lib/webkitmessageviewstyle.h"
#include <qutim/adiumchat/chatviewfactory.h>
#include <qutim/adiumchat/chatsessionimpl.h>
#include <QWebPage>
#include <QWebElement>
#include <QVariant>
#include <QWeakPointer>

class QDeclarativeEngine;
class QDeclarativeItem;
class QDeclarativeContext;

namespace Adium {

class WebViewLoaderLoop : public QObject
{
    Q_OBJECT
public:
	WebViewLoaderLoop();
	~WebViewLoaderLoop();
	
	void addPage(QWebPage *page, const QString &html);
	
private slots:
	void onPageLoaded();
	void onPageDestroyed();
	
private:
	QList<QWeakPointer<QWebPage> > m_pages;
	QStringList m_htmls;
};

class WebViewController : public QWebPage, public Core::AdiumChat::ChatViewController
{
    Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewController)
public:
	WebViewController(bool isPreview = false);
	virtual ~WebViewController();
	
	virtual void setChatSession(Core::AdiumChat::ChatSessionImpl *session);
	virtual Core::AdiumChat::ChatSessionImpl *getSession() const;
	virtual void appendMessage(const qutim_sdk_0_3::Message &msg);
	virtual void clearChat();
	virtual QString quote();
	WebKitMessageViewStyle *style();
	void setDefaultFont(const QString &family, int size);
	QString defaultFontFamily() const;
	int defaultFontSize() const;
	bool eventFilter(QObject *obj, QEvent *);
	
public slots:
	QVariant evaluateJavaScript(const QString &script);
	bool zoomImage(QWebElement elem);
	void debugLog(const QString &message);
	void appendNick(const QVariant &nick);
	void contextMenu(const QVariant &nickVar);
	void appendText(const QVariant &text);
	void setTopic();
	
protected:
	bool isContentSimiliar(const qutim_sdk_0_3::Message &a, const qutim_sdk_0_3::Message &b);
	void loadSettings(bool onFly);
	void loadHistory();
	
private slots:
	void onSettingsSaved();
	void onLoadFinished();
	void onTopicChanged(const QString &topic);
	void updateTopic();
	void onContentsChanged();
	void onObjectCleared();
	void onLinkClicked(const QUrl &url);
	
private:
	QWeakPointer<qutim_sdk_0_3::ChatSession> m_session;
	QString m_styleName;
	WebKitMessageViewStyle m_style;
	bool m_isLoading;
	bool m_isPreview;
	QStringList m_pendingScripts;
	qutim_sdk_0_3::Message m_last;
	qutim_sdk_0_3::Message m_topic;
};

} // namespace Adium

#endif // QUICKCHATVIEWCONTROLLER_H

