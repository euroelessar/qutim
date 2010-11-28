/****************************************************************************
 *  chatstyle.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauroncitadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "chatstyle.h"
#include "../chatlayer/chatsessionimpl.h"
#include <QMap>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <qutim/configbase.h>
#include <QDebug>
#include <QUrl>
#include <QStringBuilder>
#include <QWebPage>
#include "../chatlayer/chatlayerimpl.h"
namespace Core
{
namespace AdiumChat
{

void initColors(QStringList &list) {
	const char *validColors[] = {
		"aqua", "aquamarine", "blue", "blueviolet", "brown", "burlywood", "cadetblue",
		"chartreuse", "chocolate", "coral", "cornflowerblue", "crimson", "cyan",
		"darkblue", "darkcyan", "darkgoldenrod", "darkgreen", "darkgrey", "darkkhaki",
		"darkmagenta", "darkolivegreen", "darkorange", "darkorchid", "darkred",
		"darksalmon", "darkseagreen", "darkslateblue", "darkslategrey", "darkturquoise",
		"darkviolet", "deeppink", "deepskyblue", "dimgrey", "dodgerblue", "firebrick",
		"forestgreen", "fuchsia", "gold", "goldenrod", "green", "greenyellow", "grey",
		"hotpink", "indianred", "indigo", "lawngreen", "lightblue", "lightcoral",
		"lightgreen", "lightgrey", "lightpink", "lightsalmon", "lightseagreen",
		"lightskyblue", "lightslategrey", "lightsteelblue", "lime", "limegreen",
		"magenta", "maroon", "mediumaquamarine", "mediumblue", "mediumorchid",
		"mediumpurple", "mediumseagreen", "mediumslateblue", "mediumspringgreen",
		"mediumturquoise", "mediumvioletred", "midnightblue", "navy", "olive",
		"olivedrab", "orange", "orangered", "orchid", "palegreen", "paleturquoise",
		"palevioletred", "peru", "pink", "plum", "powderblue", "purple", "red",
		"rosybrown", "royalblue", "saddlebrown", "salmon", "sandybrown", "seagreen",
		"sienna", "silver", "skyblue", "slateblue", "slategrey", "springgreen",
		"steelblue", "tan", "teal", "thistle", "tomato", "turquoise", "violet",
		"yellowgreen", 0
	};
	for (int i = 0; validColors[i]; i++)
		list << QLatin1String(validColors[i]);
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(QStringList, defaultSenderColors, initColors(*x))

struct ChatStyleGeneratorPrivate
{
	void readStyleFiles()
	{
		QString templateFile = style.basePath + QString("Template.html");
		QString headerFile = style.basePath + QString("Header.html");
		QString footerFile = style.basePath + QString("Footer.html");
		QString incomingFile = style.basePath + QString("Incoming/Content.html");
		QString nextIncomingFile = style.basePath + QString("Incoming/NextContent.html");
		QString outgoingFile = style.basePath + QString("Outgoing/Content.html");
		QString nextOutgoingFile = style.basePath + QString("Outgoing/NextContent.html");
		QString incomingHistoryFile = style.basePath + QString("Incoming/Context.html");
		QString nextIncomingHistoryFile = style.basePath + QString("Incoming/NextContext.html");
		QString outgoingHistoryFile = style.basePath + QString("Outgoing/Context.html");
		QString nextOutgoingHistoryFile = style.basePath + QString("Outgoing/NextContext.html");
		QString incomingActionFile = style.basePath + QString("Incoming/Action.html");
		QString outgoingActionFile = style.basePath + QString("Outgoing/Action.html");
		QString statusFile = style.basePath + QString("Status.html");
		QString mainCSSFile = style.basePath + QString("main.css");

		QFile fileAccess;

		style.templateHtml = readStyleFile(templateFile, &fileAccess);
		style.headerHtml = readStyleFile(headerFile, &fileAccess);
		style.footerHtml = readStyleFile(footerFile, &fileAccess);
		style.incomingHtml = readStyleFile(incomingFile, &fileAccess);
		style.nextIncomingHtml = readStyleFile(nextIncomingFile, &fileAccess);
		style.outgoingHtml = readStyleFile(outgoingFile, &fileAccess);
		style.nextOutgoingHtml = readStyleFile(nextOutgoingFile, &fileAccess);
		style.incomingHistoryHtml = readStyleFile(incomingHistoryFile, &fileAccess);
		style.nextIncomingHistoryHtml = readStyleFile(nextIncomingHistoryFile, &fileAccess);
		style.outgoingHistoryHtml = readStyleFile(outgoingHistoryFile, &fileAccess);
		style.nextOutgoingHistoryHtml = readStyleFile(nextOutgoingHistoryFile, &fileAccess);
		style.incomingActionHtml = readStyleFile(incomingActionFile, &fileAccess);
		style.outgoingActionHtml = readStyleFile(outgoingActionFile, &fileAccess);
		style.statusHtml = readStyleFile(statusFile, &fileAccess);
		style.mainCSS = readStyleFile(mainCSSFile, &fileAccess);
		//			Who has done it?
		//			style.incomingActionHtml.replace("%message%","%sender% %message%");
	}
	void listVariants()
	{
		QString path =  style.basePath + QLatin1String("Variants/");
		style.variants = ChatStyleGenerator::listVariants(path);
	}
	QString readStyleFile ( const QString& filePath, QFile* fileAccess )
	{
		//qDebug() << filePath << QFile::exists(filePath);
		QString out;
		if( QFile::exists(filePath) )
		{
			fileAccess->setFileName(filePath);
			fileAccess->open(QIODevice::ReadOnly);
			QTextStream templateStream(fileAccess);
			templateStream.setCodec(QTextCodec::codecForName("UTF-8"));
			out = templateStream.readAll();
			fileAccess->close();
		}
		return out;
	}
	QStringList senderColors;
	ChatStyleStruct style;
};

bool ChatStyleStruct::isValid()
{
	if (this->headerHtml.isEmpty() || this->footerHtml.isEmpty() || (!this->backgroundColor.isValid()) || this->basePath.isEmpty())
		return false;
	if (this->currentVariantPath.isEmpty() || this->incomingActionHtml.isEmpty() || this->incomingHistoryHtml.isEmpty())
		return false;
	if (this->incomingHtml.isEmpty() || this->mainCSS.isEmpty() || this->nextIncomingHistoryHtml.isEmpty() || this->nextIncomingHtml.isEmpty())
		return false;
	if (this->nextOutgoingHistoryHtml.isEmpty() || this->nextOutgoingHtml.isEmpty() || this->statusHtml.isEmpty() || this->styleName.isEmpty())
		return false;
	if (variants.isEmpty())
		return false;
	return true;
}

ChatStyleGenerator::ChatStyleGenerator(const QString &stylePath, const QString &variant_)
	:	d(new ChatStyleGeneratorPrivate)
{
	QString variant = variant_;
	QDir styleDir = stylePath;
	if(stylePath.isEmpty() || !styleDir.exists())
		return;
	styleDir.cd("Contents/Resources");

	QUrl hrefUrl(styleDir.canonicalPath());
	hrefUrl.setScheme("file");
	d->style.basePath = styleDir.absolutePath() + QDir::separator();
	d->style.baseHref = hrefUrl.toString() + QDir::separator();
	styleDir.cdUp();
	Config settings = Config(styleDir.filePath("Info.plist"));
	QByteArray array = settings.value<QString>("DefaultBackgroundColor", "FFFFFF").toLatin1();
	bool ok;
	QRgb color = array.toInt(&ok, 16);
	d->style.backgroundColor = QColor(color);
	d->style.backgroundIsTransparent = settings.value<bool>("DefaultBackgroundIsTransparent", false);
	styleDir.cdUp();
	d->style.styleName = settings.value("CFBundleName", QString());
	if(d->style.styleName.isEmpty()) // For stupid styles, TODO: check kopete styles, if them ok - remove!
		d->style.styleName = QFileInfo( styleDir.canonicalPath() ).completeBaseName();
	d->listVariants();

	QString path = variant.isEmpty() ? QString() : d->style.basePath % QLatin1Literal("Variants/") % variant % QLatin1Literal(".css");
	QFileInfo info(path);
	if (variant.isEmpty() || !info.exists())
	{
		variant = settings.value("DefaultVariant", QString());
		path = d->style.basePath % QLatin1Literal("Variants/") % variant % QLatin1Literal(".css");
		info.setFile(path);
		if(!info.exists())
		{
			d->style.defaultVariant.first = QString();
			d->style.defaultVariant.second = QString();
			return;
		}
	}
	d->style.defaultVariant.first = variant;
	d->style.defaultVariant.second = path;
}

const QStringList &ChatStyleGenerator::getSenderColors() const
{
	if (d->senderColors.isEmpty()) {
		QFile file(d->style.basePath + QLatin1String("Incoming/SenderColors.txt"));
		if (file.open(QFile::ReadOnly)) {
			d->senderColors = QString::fromUtf8(file.readAll()).split(":");
			file.close();
		}
		if (d->senderColors.isEmpty())
			d->senderColors = *defaultSenderColors();
	}
	return d->senderColors;
}

const ChatStyleStruct &ChatStyleGenerator::getChatStyle () const
{
	d->readStyleFiles();
	d->style.senderColors = getSenderColors();
	return d->style;
}


ChatStyleGenerator::~ChatStyleGenerator()
{

}


StyleVariants ChatStyleGenerator::listVariants(const QString& path)
{
	StyleVariants variants;
	QDir variantDir(path);
	variantDir.makeAbsolute();

	QStringList variantList = variantDir.entryList(QStringList("*.css"));
	QStringList::ConstIterator it, itEnd = variantList.constEnd();
	for(it = variantList.constBegin(); it != itEnd; ++it)
	{
		QString name = *it, path;
		// Retrieve only the file name.
		name = name.left(name.lastIndexOf("."));
		// variantPath is relative to basePath.
		path = QString("Variants/%1").arg(*it);
		variants.insert(name, path);
	}
	return variants;
}


StyleVariants ChatStyleGenerator::getVariants() const
{
	d->listVariants();
	return d->style.variants;
}
// new api
class ChatStyleData
{
public:
};

class ChatStylePrivate
{
public:
	QSharedPointer<ChatStyleData> data;
	ChatSessionImpl *session;
	QWebPage *page;
};

class ChatStyleFactoryPrivate
{
public:
	QMap<QString, QWeakPointer<ChatStyleData> > datas;
};

ChatStyle::ChatStyle(ChatSessionImpl *session, const QString &, const QString &) :
	d_ptr(new ChatStylePrivate)
{
	Q_D(ChatStyle);
	d->session = session;
	d->page = session->getPage();
}

ChatStyle::~ChatStyle()
{
}

QMap<QString, QString> ChatStyle::variants() const
{
	return QMap<QString, QString>();
}

QString ChatStyle::variant() const
{
	return QString();
}

void ChatStyle::setVariant(const QString &)
{
}

void ChatStyle::appendMessage(const qutim_sdk_0_3::Message &, bool)
{
}

ChatStyleFactory *ChatStyleFactory::instance()
{
	static QGlobalStatic<ChatStyleFactory> self = { Q_BASIC_ATOMIC_INITIALIZER(0), false };
	if (!self.pointer && !self.destroyed) {
		ChatStyleFactory *x = new ChatStyleFactory;
		if (!self.pointer.testAndSetOrdered(0, x))
			delete x;
		else
			static QGlobalStaticDeleter<ChatStyleFactory> cleanup(self);
	}
	return self.pointer;
}

QStringList ChatStyleFactory::styles() const
{
	return QStringList();
}

ChatStyle *ChatStyleFactory::create(ChatSessionImpl *, const QString &, const QString &)
{
	return 0;
}

ChatStyleFactory::ChatStyleFactory() : d_ptr(new ChatStyleFactoryPrivate)
{
}

ChatStyleFactory::~ChatStyleFactory()
{
}
}
}
