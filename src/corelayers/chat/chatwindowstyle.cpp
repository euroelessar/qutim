#include <QFile>
#include <QDir>
#include <QColor>
#include <QHash>
#include <QStringList>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>
#include <QSettings>
#include <QResource>
#include <QUrl>
#include <QWebPage>
#include "chatwindowstyle.h"
#include "src/systeminfo.h"
#include "src/pluginsystem.h"
#include <QApplication>

namespace AdiumMessageStyle {

class ChatWindowStyle::Private
{
public:
	QString styleName;
	StyleVariants variantsList;
	QString baseHref;
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
	bool backgroundIsTransparent;
};

class Resource: public QResource
{
	public:
	using QResource::children;
	using QResource::isFile;
	using QResource::isDir;
	Resource() : QResource(){}
};

ChatWindowStyle::ChatWindowStyle(const QString &styleName, StyleBuildMode styleBuildMode)
	: d(new Private)
{
	init(styleName, styleBuildMode);
}

ChatWindowStyle::ChatWindowStyle(const QString &styleName, const QString &variantPath, StyleBuildMode styleBuildMode)
	: d(new Private)
{
	d->currentVariantPath = variantPath;
	init(styleName, styleBuildMode);
}

void ChatWindowStyle::init(const QString &styleName, StyleBuildMode styleBuildMode)
{
	QDir styleDir = styleName;
	if(!styleName.startsWith(':') && !styleName.isEmpty() && styleDir.exists())
	{
		d->baseHref = styleDir.canonicalPath() + QDir::separator();
		styleDir.cdUp();
		qDebug() << styleDir.filePath("Info.plist");
		{
			QSettings settings(styleDir.filePath("Info.plist"), PluginSystem::instance().plistFormat());
			bool ok = false;
			QRgb color = settings.value("DefaultBackgroundColor", "ffffff").toString().toInt(&ok, 16);
			d->backgroundColor = QColor(ok ? color : 0xffffff);
			d->backgroundIsTransparent = settings.value("DefaultBackgroundIsTransparent", false).toBool();
		}
		styleDir.cdUp();
		d->styleName = QFileInfo( styleDir.canonicalPath() ).completeBaseName();
	}
	else
	{
		d->styleName = "Default";
		d->baseHref = ":/style/webkitstyle/";
		d->backgroundColor = QColor(0xffffff);
		d->backgroundIsTransparent = false;
	}

	readStyleFiles();
	if(styleBuildMode & StyleBuildNormal)
	{
		listVariants();
	}
}

ChatWindowStyle::~ChatWindowStyle()
{
	delete d;
}

void ChatWindowStyle::preparePage(QWebPage *page) const
{
	qDebug() << d->backgroundIsTransparent;
	QPalette palette = page->palette();
	if(d->backgroundIsTransparent)
	{
		palette.setBrush(QPalette::Base, Qt::transparent);
		if(page->view())
			page->view()->setAttribute(Qt::WA_OpaquePaintEvent, false);
	}
	else
	{
		palette.setBrush(QPalette::Base, d->backgroundColor);
	}
	page->setPalette(palette);
}

ChatWindowStyle::StyleVariants ChatWindowStyle::getVariants()
{
	// If the variantList is empty, list available variants.
	if( d->variantsList.isEmpty() )
	{
		listVariants();
	}
	return d->variantsList;
}

QString ChatWindowStyle::getStyleName() const
{
	return d->styleName;
}

QString ChatWindowStyle::getStyleBaseHref() const
{
	return d->baseHref.startsWith(':') ? "qrc"+d->baseHref : QUrl::fromLocalFile(d->baseHref).toString();
//#if defined(Q_OS_WIN32)
//	return "file:///"+d->baseHref;
//#else
//	return "file://"+d->baseHref;
//#endif
}

QString ChatWindowStyle::getTemplateHtml() const
{
	return d->templateHtml;
}

QString ChatWindowStyle::getHeaderHtml() const
{
	return d->headerHtml;
}

QString ChatWindowStyle::getFooterHtml() const
{
	return d->footerHtml;
}

QString ChatWindowStyle::getIncomingHtml() const
{
	return d->incomingHtml;
}

QString ChatWindowStyle::getNextIncomingHtml() const
{
	return d->nextIncomingHtml;
}

QString ChatWindowStyle::getOutgoingHtml() const
{
	return d->outgoingHtml;
}

QString ChatWindowStyle::getNextOutgoingHtml() const
{
	return d->nextOutgoingHtml;
}

QString ChatWindowStyle::getIncomingHistoryHtml() const
{
	return d->incomingHistoryHtml;
}

QString ChatWindowStyle::getNextIncomingHistoryHtml() const
{
	return d->nextIncomingHistoryHtml;
}

QString ChatWindowStyle::getOutgoingHistoryHtml() const
{
	return d->outgoingHistoryHtml;
}

QString ChatWindowStyle::getNextOutgoingHistoryHtml() const
{
	return d->nextOutgoingHistoryHtml;
}

QString ChatWindowStyle::getIncomingActionHtml() const
{
	return d->incomingActionHtml;
}

QString ChatWindowStyle::getOutgoingActionHtml() const
{
	return d->outgoingActionHtml;
}

QString ChatWindowStyle::getStatusHtml() const
{
	return d->statusHtml;
}

QString ChatWindowStyle::getMainCSS() const
{
	return d->mainCSS;
}

void ChatWindowStyle::listVariants()
{
	QString variantDirPath = d->baseHref + QString::fromUtf8("Variants/");
	
	if(d->baseHref.startsWith(':'))
	{
		Resource variantDir;
		variantDir.setFileName(variantDirPath);
		if(variantDir.isDir())
		{
			QStringList variantList = variantDir.children();
			foreach(QString variantName, variantList)
			{
				if(variantName.endsWith(".css"))
				{
					QString variantPath = "Variants/"+variantName;
					variantName = variantName.left(variantName.lastIndexOf("."));	
					d->variantsList.insert(variantName, variantPath);
				}
			}
		}
	}
	else
	{
		QDir variantDir(variantDirPath);
		variantDir.makeAbsolute();

		QStringList variantList = variantDir.entryList(QStringList("*.css"));
		QStringList::ConstIterator it, itEnd = variantList.constEnd();
		QString compactVersionPrefix("_compact_");
		for(it = variantList.constBegin(); it != itEnd; ++it)
		{
			QString variantName = *it, variantPath;
			// Retrieve only the file name.
			variantName = variantName.left(variantName.lastIndexOf("."));
			QString compactVersionFilename = *it;
			QString compactVersionPath = variantDirPath + compactVersionFilename.prepend( compactVersionPrefix );
			// variantPath is relative to baseHref.
			variantPath = QString("Variants/%1").arg(*it);
			d->variantsList.insert(variantName, variantPath);
		}
	}
}

inline void appendStr(QString &str, const QString &res, int length)
{
	length -= res.length();
	while(length-->0)
		str += QChar(' ');
	str += res;
}

inline void appendInt(QString &str, int number, int length)
{
	int n = number;
	length--;
	while(n/=10)
		length--;
	while(length-->0)
		str += QChar('0');
	str += QString::number(number);
}

#define TRIM_LENGTH(NUM) \
	while(length > NUM) \
	{ \
		finishStr(str, week_date, date, time, c, NUM); \
		length -= NUM; \
	}

//class Test
//{
//public:
//	Test(const QString &str)
//	{
//		if(!qApp)
//			new QApplication(0, 0);
//		static QDateTime date_time = QDateTime::currentDateTime();
//		qDebug() << ChatWindowStyle::convertTimeDate(str, date_time);
//	}
//};
////yyyy.MM.dd G 'at' HH:mm:ss zzz	1996.07.10 AD at 15:08:56 PDT
////EEE, MMM d, ''yy	Wed, July 10, '96
////h:mm a	12:08 PM
////hh 'o''clock' a, zzzz	12 o'clock PM, Pacific Daylight Time
////K:mm a, z	0:00 PM, PST
////yyyyy.MMMM.dd GGG hh:mm aaa	01996.July.10 AD 12:08 PM
//
//
//static Test t1("yyyy.MM.dd G 'at' HH:mm:ss zzz");
//static Test t2("EEE, MMM d, ''yy");
//static Test t3("h:mm a");
//static Test t4("hh 'o''clock' a, zzzz");
//static Test t5("K:mm a, z");
//static Test t6("yyyyy.MMMM.dd GGG hh:mm aaa");
//static Test t7("%H:%M:%S");

void WeekDate::setDate(const QDate &date)
{
	m_week = date.weekNumber(&m_year);
	m_day = date.dayOfWeek();
}

// http://unicode.org/reports/tr35/tr35-6.html#Date_Format_Patterns
inline void finishStr(QString &str, const WeekDate &week_date, const QDate &date, const QTime &time, QChar c, int length)
{
	if(length <= 0)
		return;
	switch(c.unicode())
	{
	case L'G':{
		bool ad = date.year() > 0;
		if(length < 4)
			str += ad ? "AD" : "BC";
		else if(length == 4)
			str += ad ? "Anno Domini" : "Before Christ";
		else
			str += ad ? "A" : "B";
		break;}
	case L'y':
		if(length == 2)
			appendInt(str, date.year() % 100, 2);
		else
			appendInt(str, date.year(), length);
		break;
	case L'Y':
		appendInt(str, week_date.year(), length);
		break;
	case L'u':{
		int year = date.year();
		if(year < 0)
			year++;
		appendInt(str, date.year(), length);
		break;}
	case L'q':
	case L'Q':{
		int q = (date.month() + 2) / 3;
		if(length < 3)
			appendInt(str, q, length);
		else if(length == 3)
		{
			str += "Q";
			str += QString::number(q);
		}
		else
		{
			switch(q)
			{
			case 1:
				str += QObject::tr("1st quarter");
				break;
			case 2:
				str += QObject::tr("2nd quarter");
				break;
			case 3:
				str += QObject::tr("3rd quarter");
				break;
			case 4:
				str += QObject::tr("4th quarter");
				break;
			default:
				break;
			}
		}
		break;}
	case L'M':
	case L'L':
		if(length < 3)
			appendInt(str, date.month(), length);
		else if(length == 3)
			str += QDate::shortMonthName(date.month());
		else if(length == 4)
			str += QDate::longMonthName(date.month());
		else
			str += QDate::shortMonthName(date.month()).at(0);
		break;
	case L'w':
		TRIM_LENGTH(2);
		appendInt(str, length, week_date.week());
		break;
	case L'W':
		while(length-->0)
			str += QString::number((date.day() + 6) / 7);
		break;
	case L'd':
		TRIM_LENGTH(2);
		appendInt(str, date.day(), length);
		break;
	case L'D':
		TRIM_LENGTH(3);
		appendInt(str, date.dayOfYear(), length);
		break;
	case L'F':
		while(length-->0)
			str += QString::number(1);
		break;
	case L'g':
		appendInt(str, date.toJulianDay(), length);
		break;
	case L'c':
	case L'e':
		if(length < 3)
		{
			appendInt(str, date.dayOfWeek(), length);
			break;
		}
	case L'E':
		if(length < 4)
			str += QDate::shortDayName(date.dayOfWeek());
		else if(length == 4)
			str += QDate::longDayName(date.dayOfWeek());
		else
			str += QDate::shortDayName(date.dayOfWeek()).at(0);
		break;
	case L'a':
		str += time.hour() < 12 ? "AM" : "PM";
		break;
	case L'H':
		TRIM_LENGTH(2);
		appendInt(str, time.hour(), length);
		break;
	case L'h':
		TRIM_LENGTH(2);
		appendInt(str, time.hour() % 12, length);
		break;
	case L'K':
		TRIM_LENGTH(2);
		appendInt(str, time.hour() - 1, length);
		break;
	case L'k':
		TRIM_LENGTH(2);
		appendInt(str, time.hour() % 12 - 1, length);
		break;
	case L'm':
		TRIM_LENGTH(2);
		appendInt(str, time.minute(), length);
		break;
	case L's':
		TRIM_LENGTH(2);
		appendInt(str, time.second(), length);
		break;
	case L'S':
		str += QString::number(time.msec() / 1000.0, 'f', length).section('.', 1);
		break;
	case L'A':
		appendInt(str, QTime(0,0).msecsTo(time), length);
		break;
	case L'v':
		// I don't understand the difference
	case L'z':
		if(length < 4)
			str += SystemInfo::instance().timezoneString();
		else
			// There should be localized name, but I don't know how get it
			str += SystemInfo::instance().timezoneString();
		break;
	case L'Z':{
			if(length == 4)
				str += "GMT";
			int offset = SystemInfo::instance().timezoneOffset();
			if(offset < 0)
				str += '+';
			else
				str += '-';
			appendInt(str, qAbs((offset/60)*100 + offset%60), 4);
		break;}
	default:
		while(length-->0)
			str += c;
		break;
	}
}

QString ChatWindowStyle::convertTimeDate(const QString &mac_format, const QDateTime &datetime)
{
	QDate date = datetime.date();
	QTime time = datetime.time();
	QString str;
	if(mac_format.contains('%'))
	{
		const QChar *chars = mac_format.constData();
		bool is_percent = false;
		int length = 0;
		bool error = false;
		while((*chars).unicode() && !error)
		{
			if( is_percent )
			{
				is_percent = false;
				switch( (*chars).unicode() )
				{
				case L'%':
					str += *chars;
					break;
				case L'a':
					appendStr(str, QDate::shortDayName(date.dayOfWeek()), length);
					break;
				case L'A':
					appendStr(str, QDate::longDayName(date.dayOfWeek()), length);
					break;
				case L'b':
					appendStr(str, QDate::shortMonthName(date.day()), length);
					break;
				case L'B':
					appendStr(str, QDate::longMonthName(date.day()), length);
					break;
				case L'c':
					appendStr(str, QLocale::system().toString(datetime), length);
					break;
				case L'd':
					appendInt(str, date.day(), length > 0 ? length : 2);
					break;
				case L'e':
					appendInt(str, date.day(), length);
					break;
				case L'F':
					appendInt(str, time.msec(), length > 0 ? length : 3);
					break;
				case L'H':
					appendInt(str, time.hour(), length > 0 ? length : 2);
					break;
				case L'I':
					appendInt(str, time.hour() % 12, length > 0 ? length : 2);
					break;
				case L'j':
					appendInt(str, date.dayOfYear(), length > 0 ? length : 3);
					break;
				case L'm':
					appendInt(str, date.month(), length > 0 ? length : 2);
					break;
				case L'M':
					appendInt(str, time.minute(), length > 0 ? length : 2);
					break;
				case L'p':
					appendStr(str, time.hour() < 12 ? "AM" : "PM", length);
					break;
				case L'S':
					appendInt(str, time.second(), length > 0 ? length : 2);
					break;
				case L'w':
					appendInt(str, date.dayOfWeek(), length);
					break;
				case L'x':
					appendStr(str, QLocale::system().toString(date), length);
					break;
				case L'X':
					appendStr(str, QLocale::system().toString(time), length);
					break;
				case L'y':
					appendInt(str, date.year() % 100, length > 0 ? length : 2);
					break;
				case L'Y':
					appendInt(str, date.year(), length > 0 ? length : 4);
					break;
				case L'Z':
					// It should be localized, isn't it?..
					appendStr(str, SystemInfo::instance().timezoneString(), length);
					break;
				case L'z':{
					int offset = SystemInfo::instance().timezoneOffset();
					appendInt(str, (offset/60)*100 + offset%60, length > 0 ? length : 4);
					break;}
				default:
					if((*chars).isDigit())
					{
						is_percent = true;
						length *= 10;
						length += (*chars).digitValue();
					}
					else
						error = true;
				}
			}
			else if(*chars == '%')
			{
				length = 0;
				is_percent = true;
			}
			else
				str += *chars;
			chars++;
		}
		if(!error)
			return str;
		str = QString();
	}
	WeekDate week_date(date);
	QChar last;
	QChar cur;
	int length = 0;
	bool quote = false;
	const QChar *chars = mac_format.constData();
	forever
	{
		cur = *chars;
		if(cur == '\'')
		{
			if(*(chars+1) == '\'')
			{
				chars++;
				str += cur;
			}
			else
			{
				if(!quote)
					finishStr(str, week_date, date, time, last, length);
				quote = !quote;
			}
			length = 0;
		}
		else if(quote)
			str += cur;
		else
		{
			if(cur == last)
				length++;
			else
			{
				finishStr(str, week_date, date, time, last, length);
				length = 1;
			}
		}
		if(!chars->unicode())
			break;
		last = cur;
		chars++;
	}
	return str;
}

void ChatWindowStyle::readStyleFiles()
{
	QString templateFile = d->baseHref + QString("Template.html");
	QString headerFile = d->baseHref + QString("Header.html");
	QString footerFile = d->baseHref + QString("Footer.html");
	QString incomingFile = d->baseHref + QString("Incoming/Content.html");
	QString nextIncomingFile = d->baseHref + QString("Incoming/NextContent.html");
	QString outgoingFile = d->baseHref + QString("Outgoing/Content.html");
	QString nextOutgoingFile = d->baseHref + QString("Outgoing/NextContent.html");
	QString incomingHistoryFile = d->baseHref + QString("Incoming/Context.html");
	QString nextIncomingHistoryFile = d->baseHref + QString("Incoming/NextContext.html");
	QString outgoingHistoryFile = d->baseHref + QString("Outgoing/Context.html");
	QString nextOutgoingHistoryFile = d->baseHref + QString("Outgoing/NextContext.html");
	QString incomingActionFile = d->baseHref + QString("Incoming/Action.html");
	QString outgoingActionFile = d->baseHref + QString("Outgoing/Action.html");
	QString statusFile = d->baseHref + QString("Status.html");
	QString mainCSSFile = d->baseHref + QString("main.css");

	QFile fileAccess;
	// First load template file.
	if( QFile::exists(templateFile) )
	{
		fileAccess.setFileName(templateFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream templateStream(&fileAccess);
		templateStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->templateHtml = templateStream.readAll();
		fileAccess.close();


	}
	else
	{
					QResource resourceAccess("style/webkitstyle/Template.html");
		d->templateHtml = QString::fromUtf8((char*)resourceAccess.data(),resourceAccess.size());
	}
	// Load header file.
	if( QFile::exists(headerFile) )
	{
		fileAccess.setFileName(headerFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->headerHtml = headerStream.readAll();
		//qDebug() << "Header HTML: " << d->headerHtml;
		fileAccess.close();
	}
	// Load Footer file
	if( QFile::exists(footerFile) )
	{
		fileAccess.setFileName(footerFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->footerHtml = headerStream.readAll();
		//qDebug() << "Footer HTML: " << d->footerHtml;
		fileAccess.close();
	}
	// Load incoming file
	if( QFile::exists(incomingFile) )
	{
		fileAccess.setFileName(incomingFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->incomingHtml = headerStream.readAll();
//			qDebug() << "Incoming HTML: " << d->incomingHtml;
		fileAccess.close();
	}
	// Load next Incoming file
	if( QFile::exists(nextIncomingFile) )
	{
		fileAccess.setFileName(nextIncomingFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->nextIncomingHtml = headerStream.readAll();
		//qDebug() << "NextIncoming HTML: " << d->nextIncomingHtml;
		fileAccess.close();
	}
	else
		d->nextIncomingHtml = d->incomingHtml;
	// Load outgoing file
	if( QFile::exists(outgoingFile) )
	{
		fileAccess.setFileName(outgoingFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->outgoingHtml = headerStream.readAll();
		//qDebug() << "Outgoing HTML: " << d->outgoingHtml;
		fileAccess.close();
	}
	else
		d->outgoingHtml = d->incomingHtml;
	// Load next outgoing file
	if( QFile::exists(nextOutgoingFile) )
	{
		fileAccess.setFileName(nextOutgoingFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->nextOutgoingHtml = headerStream.readAll();
		//qDebug() << "NextOutgoing HTML: " << d->nextOutgoingHtml;
		fileAccess.close();
	}
	else
					d->nextOutgoingHtml = d->outgoingHtml;
	// Load incoming history file
	if( QFile::exists(incomingHistoryFile) )
	{
		fileAccess.setFileName(incomingHistoryFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->incomingHistoryHtml = headerStream.readAll();
//			qDebug() << "Incoming HTML: " << d->incomingHtml;
		fileAccess.close();
	}
	else
		d->incomingHistoryHtml = d->incomingHtml;
	// Load next Incoming history file
	if( QFile::exists(nextIncomingHistoryFile) )
	{
		fileAccess.setFileName(nextIncomingHistoryFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->nextIncomingHistoryHtml = headerStream.readAll();
		//qDebug() << "NextIncoming HTML: " << d->nextIncomingHtml;
		fileAccess.close();
	}
	else
		d->nextIncomingHistoryHtml = d->nextIncomingHtml;
	// Load outgoing history file
	if( QFile::exists(outgoingHistoryFile) )
	{
		fileAccess.setFileName(outgoingHistoryFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->outgoingHistoryHtml = headerStream.readAll();
		//qDebug() << "Outgoing HTML: " << d->outgoingHtml;
		fileAccess.close();
	}
	else
		d->outgoingHistoryHtml = d->outgoingHtml;
	// Load next outgoing history file
	if( QFile::exists(nextOutgoingHistoryFile) )
	{
		fileAccess.setFileName(nextOutgoingHistoryFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->nextOutgoingHistoryHtml = headerStream.readAll();
		//qDebug() << "NextOutgoing HTML: " << d->nextOutgoingHtml;
		fileAccess.close();
	}
	else
		d->nextOutgoingHistoryHtml = d->nextOutgoingHtml;
	// Load status file
	if( QFile::exists(statusFile) )
	{
		fileAccess.setFileName(statusFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->statusHtml = headerStream.readAll();
		//qDebug() << "Status HTML: " << d->statusHtml;
		fileAccess.close();
	}
	// Load incoming action file
	if( QFile::exists(incomingActionFile) )
	{
		fileAccess.setFileName(incomingActionFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->incomingActionHtml = headerStream.readAll();
		fileAccess.close();
	}
	else
	{
		d->incomingActionHtml = d->statusHtml;
		d->incomingActionHtml = d->incomingActionHtml.replace("%message%","%sender% %message%");
	}
	// Load outgoing action file
	if( QFile::exists(outgoingActionFile) )
	{
		fileAccess.setFileName(outgoingActionFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->outgoingActionHtml = headerStream.readAll();
		//qDebug() << "Action HTML: " << d->actionHtml;
		fileAccess.close();
	}
	else
		d->outgoingActionHtml = d->incomingActionHtml;
	// Load main.css file
	if( QFile::exists(mainCSSFile) )
	{
		fileAccess.setFileName(mainCSSFile);
		fileAccess.open(QIODevice::ReadOnly);
		QTextStream headerStream(&fileAccess);
		headerStream.setCodec(QTextCodec::codecForName("UTF-8"));
		d->mainCSS = headerStream.readAll();
		//qDebug() << "mainCSS: " << d->mainCSS;
		fileAccess.close();
	}
}

void ChatWindowStyle::reload()
{
	d->variantsList.clear();
	readStyleFiles();
	listVariants();
}

}
