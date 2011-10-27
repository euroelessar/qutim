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


#ifndef URLPREVIEW_MESSAGEHANDLER_H
#define URLPREVIEW_MESSAGEHANDLER_H
#include <qutim/messagehandler.h>
#include <QSslError>
#include <QSize>


class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;
namespace UrlPreview {

enum PreviewFlag
{
	PreviewImages = 0x1,
	PreviewYoutube = 0x2,
	DisableTextHtml = 0x4
};
Q_DECLARE_FLAGS(PreviewFlags, PreviewFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(UrlPreview::PreviewFlags)

class UrlHandler : public QObject, public qutim_sdk_0_3::MessageHandler
{
    Q_OBJECT
public:
	explicit UrlHandler();
protected:
	virtual qutim_sdk_0_3::MessageHandler::Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);
public slots:
	void loadSettings();
private slots:
	void netmanFinished(QNetworkReply *);
	void authenticationRequired(QNetworkReply *, QAuthenticator *);
	void netmanSslErrors(QNetworkReply *, const QList<QSslError> &);
private:
	static const QRegExp &getLinkRegExp();
	void checkLink(QString &url, qutim_sdk_0_3::ChatUnit *from, qint64 id);
	QNetworkAccessManager *m_netman;
	PreviewFlags m_flags;
	QString m_template;
	QString m_imageTemplate;
	QString m_youtubeTemplate;
	QString m_html5AudioTemplate;
	QString m_html5VideoTemplate;
	QSize m_maxImageSize;
	quint64 m_maxFileSize;
	bool m_enableYoutubePreview;
	bool m_enableImagesPreview;
	bool m_enableHTML5Audio;
	bool m_enableHTML5Video;
};

} // namespace UrlPreview

#endif // URLPREVIEW_MESSAGEHANDLER_H
