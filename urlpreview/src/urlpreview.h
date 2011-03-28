/*
    UrlPreviewPlugin

  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
  (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef urlpreviewPLUGIN_H
#define urlpreviewPLUGIN_H
#include <qutim/plugin.h>

class QSslError;
class QAuthenticator;
class QNetworkReply;
class QNetworkAccessManager;

namespace qutim_sdk_0_3 {
class ChatUnit;
class ChatSession;
class Message;
}

namespace UrlPreview {

using namespace qutim_sdk_0_3;

enum PreviewFlag
{
	PreviewImages = 0x1,
	PreviewYoutube = 0x2,
	DisableTextHtml = 0x4,
};
Q_DECLARE_FLAGS(PreviewFlags,PreviewFlag);

class UrlPreviewPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "UrlPreview")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private slots:
	void sessionCreated(qutim_sdk_0_3::ChatSession*);
	void processMessage(qutim_sdk_0_3::Message*);
	void netmanFinished ( QNetworkReply * );
	void authenticationRequired ( QNetworkReply * , QAuthenticator * );
	void netmanSslErrors ( QNetworkReply * , const QList<QSslError> & );
private:
	static const QRegExp &getLinkRegExp();
	void checkLink(QString &url, ChatUnit *from, qint64 id);
	QNetworkAccessManager *m_netman;
	PreviewFlags m_flags;
	QString m_template;
	QString m_image_template;
	QString m_youtube_template;
	QSize m_max_image_size;
	quint64 m_maxfilesize;
};
}
#endif
