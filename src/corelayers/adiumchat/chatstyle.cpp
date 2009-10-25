#include "chatstyle.h"
#include <QMap>
#include <QDir>
#include <QTextStream>
#include <QTextCodec>
#include <libqutim/configbase.h>
#include <QDebug>

namespace AdiumChat
{
	using namespace qutim_sdk_0_3;
	
	bool ChatStyle::isValid()
	{
	if (this->headerHtml.isEmpty() || this->footerHtml.isEmpty() || (!this->backgroundColor.isValid()) || this->baseHref.isEmpty())
		return false;
	if (this->currentVariantPath.isEmpty() || this->incomingActionHtml.isEmpty() || this->incomingHistoryHtml.isEmpty())
		return false;
	if (this->incomingHtml.isEmpty() || this->mainCSS.isEmpty() || this->nextIncomingHistoryHtml.isEmpty() || this->nextIncomingHtml.isEmpty())
		return false;
	if (this->nextOutgoingHistoryHtml.isEmpty() || this->nextOutgoingHtml.isEmpty() || this->statusHtml.isEmpty() || this->styleName.isEmpty())
		return false;
	return true;
	};

	struct ChatStyleGeneratorPrivate
	{
		void readStyleFiles()
		{
			QString templateFile = style.baseHref + QString("Template.html");
			QString headerFile = style.baseHref + QString("Header.html");
			QString footerFile = style.baseHref + QString("Footer.html");
			QString incomingFile = style.baseHref + QString("Incoming/Content.html");
			QString nextIncomingFile = style.baseHref + QString("Incoming/NextContent.html");
			QString outgoingFile = style.baseHref + QString("Outgoing/Content.html");
			QString nextOutgoingFile = style.baseHref + QString("Outgoing/NextContent.html");
			QString incomingHistoryFile = style.baseHref + QString("Incoming/Context.html");
			QString nextIncomingHistoryFile = style.baseHref + QString("Incoming/NextContext.html");
			QString outgoingHistoryFile = style.baseHref + QString("Outgoing/Context.html");
			QString nextOutgoingHistoryFile = style.baseHref + QString("Outgoing/NextContext.html");
			QString incomingActionFile = style.baseHref + QString("Incoming/Action.html");
			QString outgoingActionFile = style.baseHref + QString("Outgoing/Action.html");
			QString statusFile = style.baseHref + QString("Status.html");
			QString mainCSSFile = style.baseHref + QString("main.css");

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

			style.incomingActionHtml.replace("%message%","%sender% %message%");
		}
		void listVariants()
		{
			QString path =  style.baseHref + QString::fromUtf8("Variants/");
			QDir variantDir(path);
			variantDir.makeAbsolute();

			QStringList variantList = variantDir.entryList(QStringList("*.css"));
			QStringList::ConstIterator it, itEnd = variantList.constEnd();
			for(it = variantList.constBegin(); it != itEnd; ++it)
			{
				QString name = *it, path;
				// Retrieve only the file name.
				name = name.left(name.lastIndexOf("."));
				// variantPath is relative to baseHref.
				path = QString("Variants/%1").arg(*it);
				style.variants.insert(name, path);
			}
		};
		QString readStyleFile ( const QString& filePath, QFile* fileAccess )
		{
			if( QFile::exists(filePath) )
			{
				fileAccess->setFileName(filePath);
				fileAccess->open(QIODevice::ReadOnly);
				QTextStream templateStream(fileAccess);
				templateStream.setCodec(QTextCodec::codecForName("UTF-8"));
				fileAccess->close();
				return templateStream.readAll();
			}
			return QString();
		};
		ChatStyle style;
	};
	
	ChatStyleGenerator::ChatStyleGenerator ( const QString& stylePath, const QString& variant )
	{
		QDir styleDir = stylePath;
		if(stylePath.isEmpty() || !styleDir.exists())
			return;

		d->style.baseHref = styleDir.canonicalPath() + QDir::separator();
		styleDir.cdUp();
		qDebug() << styleDir.filePath("Info.plist");
		ConfigGroup settings = Config(styleDir.filePath("Info.plist")).group("");
		QRgb color = settings.value<int>("DefaultBackgroundColor", 0xffffff);
		d->style.backgroundColor = QColor(color);
		d->style.backgroundIsTransparent = settings.value<bool>("DefaultBackgroundIsTransparent", false);
		styleDir.cdUp();
		d->style.styleName = QFileInfo( styleDir.canonicalPath() ).completeBaseName();

		d->readStyleFiles();
		if (variant.isEmpty())
			d->listVariants();
		else
		{
			//TODO
		}
	}

	void ChatStyleGenerator::reload()
	{
		d->style.variants.clear();
		d->listVariants();
		d->readStyleFiles();
	}

	
	ChatStyle ChatStyleGenerator::getChatStyle () const
	{
		return d->style;
	}

}