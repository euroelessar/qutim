/****************************************************************************
 *  chatstyle.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <QMap>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <libqutim/configbase.h>
#include <QDebug>
#include <QUrl>
#include <QStringBuilder>
#include "chatlayerimpl.h"

namespace AdiumChat
{
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
		ChatStyle style;
	};

	bool ChatStyle::isValid()
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
		QByteArray array = settings.value<QByteArray>("DefaultBackgroundColor", "FFFFFF");
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

	ChatStyle ChatStyleGenerator::getChatStyle () const
	{
		d->readStyleFiles();
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


}
