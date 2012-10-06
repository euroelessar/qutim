/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "libqutim_global.h"
#include "json.h"
#include "systeminfo.h"
#include "utils.h"
#include <QDate>
#include <QLocale>
#include <QDesktopWidget>
#include <QApplication>
#include <QUrl>

namespace qutim_sdk_0_3
{
	class WeekDate
	{
	public:
		inline WeekDate(const QDate &date) { setDate(date); }
		inline WeekDate(int y, int m, int d) { setDate(QDate(y, m, d)); }
		inline int year() const { return m_year; }
		inline int week() const { return m_week; }
		inline int day() const { return m_day; }
		void setDate(const QDate &date)
		{
			m_week = date.weekNumber(&m_year);
			m_day = date.dayOfWeek();
		}
	private:
		int m_year;
		int m_week;
		int m_day;
	};

	inline void appendStr(QString &str, const QString &res, int length)
	{
		length -= res.length();
		while (length-->0)
			str += QChar(' ');
		str += res;
	}

	inline void appendInt(QString &str, int number, int length)
	{
		int n = number;
		length--;
		while (n /= 10)
			length--;
		while (length --> 0)
			str += QLatin1Char('0');
		str += QString::number(number);
	}

#define TRIM_LENGTH(NUM) \
	while (length > NUM) { \
	   finishStr(str, week_date, date, time, c, NUM); \
	   length -= NUM; \
	}

	// TODO: On MacOS X use native methods
	// http://unicode.org/reports/tr35/tr35-6.html#Date_Format_Patterns
	inline void finishStr(QString &str, const WeekDate &week_date, const QDate &date, const QTime &time, QChar c, int length)
	{
		if (length <= 0)
			return;
		switch (c.unicode()) {
		case L'G': {
			bool ad = date.year() > 0;
			if (length < 4)
				str += ad ? "AD" : "BC";
			else if (length == 4)
				str += ad ? "Anno Domini" : "Before Christ";
			else
				str += ad ? "A" : "B";
			break;
		}
		case L'y':
			if (length == 2)
				appendInt(str, date.year() % 100, 2);
			else
				appendInt(str, date.year(), length);
			break;
		case L'Y':
			appendInt(str, week_date.year(), length);
			break;
		case L'u': {
			int year = date.year();
			if (year < 0)
				year++;
			appendInt(str, date.year(), length);
			break;
		}
		case L'q':
		case L'Q': {
			int q = (date.month() + 2) / 3;
			if (length < 3) {
				appendInt(str, q, length);
			} else if (length == 3) {
				str += "Q";
				str += QString::number(q);
			} else {
				switch (q) {
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
			break;
		}
		case L'M':
		case L'L':
			if (length < 3)
				appendInt(str, date.month(), length);
			else if (length == 3)
				str += QDate::shortMonthName(date.month());
			else if (length == 4)
				str += QDate::longMonthName(date.month());
			else
				str += QDate::shortMonthName(date.month()).at(0);
			break;
		case L'w':
			TRIM_LENGTH(2);
			appendInt(str, length, week_date.week());
			break;
		case L'W':
			while (length-->0)
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
			while (length-->0)
				str += QString::number(1);
			break;
		case L'g':
			appendInt(str, date.toJulianDay(), length);
			break;
		case L'c':
		case L'e':
			if (length < 3) {
				appendInt(str, date.dayOfWeek(), length);
				break;
			}
		case L'E':
			if (length < 4)
				str += QDate::shortDayName(date.dayOfWeek());
			else if (length == 4)
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
			appendInt(str, QTime(0, 0).msecsTo(time), length);
			break;
		case L'v':
			// I don't understand the difference
		case L'z':
			if (length < 4)
				str += qutim_sdk_0_3::SystemInfo::getTimezone();
			else
				// There should be localized name, but I don't know how get it
				str += qutim_sdk_0_3::SystemInfo::getTimezone();
			break;
		case L'Z': {
				if (length == 4)
					str += QLatin1String("GMT");
				int offset = qutim_sdk_0_3::SystemInfo::getTimezoneOffset();
				if (offset < 0)
					str += QLatin1Char('+');
				else
					str += QLatin1Char('-');
				appendInt(str, qAbs((offset/60)*100 + offset%60), 4);
				break;
			}
		default:
			while (length-->0)
				str += c;
			break;
		}
	}

	LIBQUTIM_EXPORT QString convertTimeDate(const QString& mac_format, const QDateTime& datetime)
	{
		QDate date = datetime.date();
		QTime time = datetime.time();
		QString str;
		// TODO: may be we should use system methods on *NIX-compatible systems?
		if (mac_format.contains('%')) {
			const QChar *chars = mac_format.constData();
			bool is_percent = false;
			int length = 0;
			bool error = false;
			while ((*chars).unicode() && !error) {
				if ( is_percent ) {
					is_percent = false;
					switch (chars->unicode()) {
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
						appendStr(str, QDate::shortMonthName(date.month()), length);
						break;
					case L'B':
						appendStr(str, QDate::longMonthName(date.month()), length);
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
						appendStr(str, SystemInfo::getTimezone(), length);
						break;
					case L'z': {
						int offset = SystemInfo::getTimezoneOffset();
						appendInt(str, (offset/60)*100 + offset%60, length > 0 ? length : 4);
						break;
					}
					default:
						if ((*chars).isDigit()) {
							is_percent = true;
							length *= 10;
							length += (*chars).digitValue();
						}
						else
							error = true;
					}
				} else if (*chars == QLatin1Char('%')) {
					length = 0;
					is_percent = true;
				} else
					str += *chars;
				chars++;
			}
			if (!error)
				return str;
			str = QString();
		}
		WeekDate week_date(date);
		QChar last;
		QChar cur;
		int length = 0;
		bool quote = false;
		const QChar *chars = mac_format.constData();
		forever {
			cur = *chars;
			if (cur == QLatin1Char('\'')) {
				if (*(chars+1) == QLatin1Char('\'')) {
					chars++;
					str += cur;
				} else {
					if (!quote)
						finishStr(str, week_date, date, time, last, length);
					quote = !quote;
				}
				length = 0;
			} else if (quote) {
				str += cur;
			} else {
				if (cur == last) {
					length++;
				} else {
					finishStr(str, week_date, date, time, last, length);
					length = 1;
				}
			}
			if (!chars->unicode())
				break;
			last = cur;
			chars++;
		}
		return str;
	}
	
	LIBQUTIM_EXPORT QString &validateCpp(QString &text)
	{
		QString txt;
		txt.reserve(text.size() * 1.2);
		for (int i = 0; i < text.size(); i++) {
			switch (text.at(i).unicode()) {
			case L'\"':
				txt += QLatin1String("\\\"");
				break;
			case L'\n':
				txt += QLatin1String("\\n");
				break;
			case L'\t':
				txt += QLatin1String("\\t");
				break;
			case L'\\':
				txt += QLatin1String("\\\\");
				break;
			case L'\r':
				txt += QLatin1String("\\r");
				break;
			default:
				txt += text.at(i);
				break;
			}
		}
		return (text = txt);
	}

	void centerizeWidget(QWidget *widget)
	{
		QRect rect = QApplication::desktop()->screenGeometry(QCursor::pos());
		QPoint position(rect.left() + rect.width() / 2 - widget->size().width() / 2,
						rect.top() + rect.height() / 2 - widget->size().height() / 2);
		widget->move(position);
	}

	UrlParser::UrlTokenList UrlParser::tokenize(const QString &text, Flags flags)
	{
		UrlTokenList result;
		static QRegExp linkRegExp("([a-zA-Z0-9\\-\\_\\.]+@([a-zA-Z0-9\\-\\_]+\\.)+[a-zA-Z]+)|"
		                          "([a-z]+(\\+[a-z]+)?://|www\\.)"
		                          "[\\w-]+(\\.[\\w-]+)*\\.\\w+"
		                          "(:\\d+)?"
		                          "(/[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@'&=~-]*"
		                          "(\\?[\\w\\+\\.\\[\\]!%\\$/\\(\\),:;@\\'&=~-]*)?"
		                          "(#[\\w\\+\\.\\[\\]!%\\$/\\\\\\(\\)\\|,:;@&=~-]*)?)?",
		                          Qt::CaseInsensitive);
		Q_ASSERT(linkRegExp.isValid());
		QList<QPair<int, int> > tags;
		int currentTag = 0;
		if (flags & Html) {
			enum TagParserState {
				AtText,
				AtTag,
				AtSingleQuote,
				AtDoubleQuote
			};
			TagParserState state = AtText;
			int start = 0;
			for (int i = 0; i < text.size(); ++i) {
				QChar ch = text.at(i);
				switch (state) {
				case AtText:
					if (ch == QLatin1Char('<')) {
						state = AtTag;
						start = i;
					}
					break;
				case AtTag:
					if (ch == QLatin1Char('>')) {
						tags << qMakePair(start, i);
						state = AtText;
					} else if (ch == QLatin1Char('\'')) {
						state = AtSingleQuote;
					} else if (ch == QLatin1Char('\"')) {
						state = AtDoubleQuote;
					}
					break;
				case AtSingleQuote:
					if (ch == QLatin1Char('\''))
						state = AtTag;
					break;
				case AtDoubleQuote:
					if (ch == QLatin1Char('\"'))
						state = AtTag;
					break;
				}
			}
		}
		int pos = 0;
		int lastPos = 0;
		while (((pos = linkRegExp.indexIn(text, pos)) != -1)) {
			QString link = linkRegExp.cap(0);
			while (currentTag < tags.size() && tags.at(currentTag).second < pos)
				currentTag++;
			if (currentTag < tags.size()) {
				const QPair<int, int> &pair = tags.at(currentTag);
				int left = qBound(pair.first, pos, pair.second);
				int right = qBound(pair.first, pos + link.size(), pair.second);
				if (left != right) {
					pos += link.size();
					continue;
				}
			}
			UrlToken tok = { text.midRef(lastPos, pos - lastPos), QString() };
			if (!tok.text.isEmpty()) {
				if (!result.isEmpty() && result.last().url.isEmpty()) {
					QStringRef tmp = result.last().text;
					result.last().text = QStringRef(tmp.string(), tmp.position(), tmp.size() + tok.text.size());
				} else {
					result << tok;
				}
			}
			tok.text = text.midRef(pos, link.size());
			pos += link.size();
			if (link.startsWith(QLatin1String("www."), Qt::CaseInsensitive))
				link.prepend(QLatin1String("http://"));
			else if(!link.contains(QLatin1String("//")))
				link.prepend(QLatin1String("mailto:"));
			tok.url = link;
			result << tok;
			lastPos = pos;
		}
		if (!result.isEmpty() && result.last().url.isEmpty()) {
			result.last().text = text.midRef(result.last().text.position());
		} else {
			UrlToken tok = { text.midRef(lastPos), QString() };
			result << tok;
		}
		return result;
	}
	
	QString UrlParser::parseUrls(const QString &text, Flags flags)
	{
		const QString hrefTemplate(QLatin1String("<a href='%1' title='%2' target='_blank'>%3</a>"));
		QString html;
		foreach (const UrlToken &token, tokenize(text, flags)) {
			if (token.url.isEmpty()) {
				html += token.text.toString();
			} else {
				QUrl url = QUrl::fromUserInput(token.url);
				QByteArray urlEncoded = url.toEncoded();
				html += hrefTemplate.arg(QString::fromLatin1(urlEncoded, urlEncoded.size()),
				                         url.toString(),
				                         token.text.toString());
			}
		}
		return html;
	}
}

