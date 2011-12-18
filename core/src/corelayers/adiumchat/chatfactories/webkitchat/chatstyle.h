/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef CHATSTYLE_H
#define CHATSTYLE_H
#include <QStringList>
#include <QColor>
#include <QMap>
#include <QPair>

namespace qutim_sdk_0_3
{
class Message;
}
class QWebPage;
namespace Core
{
namespace AdiumChat
{

class ChatSessionImpl;
typedef QMap<QString,QString> StyleVariants;
enum StyleVariableType {COLOR, FONT, BACKGROUND, NUMERIC};
struct StyleVariable
{
	QString name;
	QString label;
	QString desc;
	QString value;
	StyleVariableType type;
	QString range;
};
typedef QList<StyleVariable> StyleVariables;
struct ChatStyleStruct
{
	QString styleName;
	QString baseHref;
	QString basePath;
	QString currentVariantPath;
	QString templateHtml;
	QString headerHtml;
	QString footerHtml;
	QString incomingHtml;
	QString nextIncomingHtml;
	QString outgoingHtml;
	QString nextOutgoingHtml;
	QString incomingHistoryHtml;
	QString nextIncomingHistoryHtml;
	QString outgoingHistoryHtml;
	QString nextOutgoingHistoryHtml;
	QString incomingActionHtml;
	QString outgoingActionHtml;
	QString statusHtml;
	QString mainCSS;
	QColor backgroundColor;
	QStringList senderColors;
	QPair<QString, QString> defaultVariant;
	StyleVariants variants;
	bool backgroundIsTransparent;
	bool isValid();
};

struct ChatStyleGeneratorPrivate;
class ChatStyleGenerator
{
public:
	ChatStyleGenerator (const QString &stylePath, const QString &variant = QString());
	~ChatStyleGenerator();
	const QStringList &getSenderColors() const;
	const ChatStyleStruct &getChatStyle () const;
	StyleVariants getVariants() const;
	static StyleVariants listVariants (const QString &path);
private:
	QScopedPointer<ChatStyleGeneratorPrivate> d;
};

class ChatStylePrivate;
class ChatStyleFactoryPrivate;
class ChatStyle
{
	Q_DISABLE_COPY(ChatStyle)
	Q_DECLARE_PRIVATE(ChatStyle)
public:
	ChatStyle(ChatSessionImpl *impl,QWebPage *page);
	~ChatStyle();

	QMap<QString, QString> variants() const;
	QString variant() const;
	void setVariant(const QString &variant);
	void appendMessage(const qutim_sdk_0_3::Message &msg, bool sameSender);
private:
	QScopedPointer<ChatStylePrivate> d_ptr;
};

class ChatStyleFactory
{
	Q_DISABLE_COPY(ChatStyleFactory)
	Q_DECLARE_PRIVATE(ChatStyleFactory)
public:
	static ChatStyleFactory *instance();

	QStringList styles() const;
	ChatStyle *create(ChatSessionImpl *impl, const QString &stylePath,
					  const QString &variant = QString());
private:
	ChatStyleFactory();
	~ChatStyleFactory();
	QScopedPointer<ChatStyleFactoryPrivate> d_ptr;
	friend class QGlobalStaticDeleter<ChatStyleFactory>;
};
}
}
#endif // CHATSTYLE_H

