/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef WEBKITMESSAGEVIEWSTYLE_H
#define WEBKITMESSAGEVIEWSTYLE_H

#include "adiumwebview_global.h"
#include <QObject>
#include <QDateTime>
#include <QStringList>
#include <QVariantMap>
#include <QColor>
#include <QCoreApplication>

namespace qutim_sdk_0_3 {
class ChatSession;
class Message;
}

class WebKitMessageViewStylePrivate;

class ADIUMWEBVIEW_EXPORT WebKitMessageViewStyle : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WebKitMessageViewStyle)
    Q_ENUMS(NameFormat WebkitBackgroundType)
public:
	enum NameFormat {
		AIDefaultName = 0,
		AIDisplayName = 1,
		AIDisplayName_ScreenName = 2,
		AIScreenName_DisplayName = 3,
		AIScreenName = 4
	};
	
	enum WebkitBackgroundType {
		BackgroundNormal = 0,
		BackgroundCenter,
		BackgroundTile,
		BackgroundTileCenter,
		BackgroundScale
	};
	
    WebKitMessageViewStyle();
    ~WebKitMessageViewStyle();

public slots:
	void setStylePath(const QString &path);
	void setCustomStyle(const QString &style);
	QString baseTemplateForChat(qutim_sdk_0_3::ChatSession *session);
	QString templateForContent(const qutim_sdk_0_3::Message &message, bool contentIsSimilar);
	QString scriptForChangingVariant();
	QString scriptForSettingCustomStyle();
	QString scriptForAppendingContent(const qutim_sdk_0_3::Message &message, bool contentIsSimilar, bool willAddMoreContentObjects, bool replaceLastContent);
	QStringList variants();
	QString defaultVariant() const;
	QString activeVariant() const;
	void setActiveVariant(const QString &variant);
	
	bool allowsCustomBackground() const;
	bool transparentDefaultBackground() const;
	bool allowsUserIcons() const;
	bool hasHeader() const;
	bool hasTopic() const;
	bool hasAction() const;
	QString defaultFontFamily() const;
	int defaultFontSize() const;
	
	bool showUserIcons() const;
	void setShowUserIcons(bool showUserIcons);
	bool showHeader() const;
	void setShowHeader(bool showHeader);
	bool showIncomingFonts() const;
	void setShowIncomingFonts(bool showIncomingFonts);
	bool showIncomingColors() const;
	void setShowIncomingColors(bool showIncomingColors);
	WebkitBackgroundType customBackgroundType() const;
	void setCustomBackgroundType(WebkitBackgroundType type);
	QString customBackgroundPath() const;
	void setCustomBackgroundPath(const QString &path);
	QColor  customBackgroundColor() const;
	void setCustomBackgroundColor(const QColor &color);
	
	QString pathForResource(const QString &name, const QString &directory = QString());
	QString loadResourceFile(const QString &name, const QString &directory = QString());
	
private:
	struct UnitData
	{
		QString id;
		QString title;
		QString avatar;
	};
	void reloadStyle();
	QString noVariantName() const;
	QString activeVariantPath() const;
	QStringList validSenderColors();
	void loadTemplates();
	void releaseResources();
	UnitData getSourceData(const qutim_sdk_0_3::Message &message);
	QString &fillKeywords(QString &inString, const qutim_sdk_0_3::Message &message, bool contentIsSimilar);
	QString &fillKeywordsForBaseTemplate(QString &inString, qutim_sdk_0_3::ChatSession *session);
	QString stringWithFormat(const QString &str, const QStringList &args);
	
	QScopedPointer<WebKitMessageViewStylePrivate> d_ptr;
};

#endif // WEBKITMESSAGEVIEWSTYLE_H
