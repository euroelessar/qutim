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

#include "emoticons.h"
#include "configbase.h"
#include "objectgenerator.h"
#include <QStringList>
#include <QSet>
#include <QHash>
#include <QImageReader>
#include <QStringBuilder>
#include <QTextDocument>
#include <QDebug>

namespace qutim_sdk_0_3
{
#define nullThemeName QStringLiteral("")

namespace Emoticons
{
struct Private
{
	~Private();
	QHash<QString, EmoticonsThemeData*> cache;
	QList<EmoticonsBackend *> backends;
};
static QScopedPointer<Private> p;

Private::~Private()
{
	qDeleteAll(backends);
}
}

struct EmoticonsThemeData : public QSharedData
{
	EmoticonsThemeData() : provider(0) {}
	EmoticonsThemeData(const EmoticonsThemeData &o) : QSharedData(o), provider(o.provider) {}
	~EmoticonsThemeData()
	{
		Emoticons::p->cache.remove(provider->themeName());
		delete provider;
	}
	EmoticonsProvider *provider;
};

struct EmoticonsProviderPrivate
{
	QStringList order;
	QHash<QString, QStringList> map;
	QHash<QChar, QList<EmoticonsProvider::Emoticon> > indexes;
};

namespace Emoticons
{
void ensurePrivate_helper()
{
	p.reset(new Private);
	GeneratorList exts = ObjectGenerator::module<EmoticonsBackend>();
	foreach (const ObjectGenerator *gen, exts)
		p->backends << gen->generate<EmoticonsBackend>();
}

inline void ensurePrivate()
{ if(!p) ensurePrivate_helper(); }
}

//	class PseudoEmoticonsProvider : public EmoticonsProvider
//	{
//	public:
//		PseudoEmoticonsProvider(EmoticonsProvider *provider)
//		{
//		}
//		virtual QHash<QString, QStringList> emoticonsMap() = 0;
//		virtual QStringList emoticonsIndexes() = 0;
//		// Warinig: matchText must be in lower-case
//		virtual QHash<QChar, QList<Emoticon> > emoticonsByChar() = 0;
//		virtual void appendEmoticon(const QString &imgPath, const QStringList &code) = 0;
//		virtual void removeEmoticon(const Emoticon &emoticon) = 0;
//	private:
//		QHash<QString, QStringList> m_map;
//	};

EmoticonsProvider::EmoticonsProvider() : p(new EmoticonsProviderPrivate)
{
}

EmoticonsProvider::~EmoticonsProvider()
{
}

QHash<QString, QStringList> EmoticonsProvider::emoticonsMap() const
{
	return p->map;
}

QStringList EmoticonsProvider::emoticonsIndexes() const
{
	return p->order;
}

QHash<QChar, QList<EmoticonsProvider::Emoticon> > EmoticonsProvider::emoticonsByChar() const
{
	return p->indexes;
}

QString EmoticonsProvider::themeName() const
{
	return QString();
}

bool EmoticonsProvider::saveTheme()
{
	return false;
}

bool EmoticonsProvider::addEmoticon(const QString &imgPath, const QStringList &codes)
{
	Q_UNUSED(imgPath);
	Q_UNUSED(codes);
	return false;
}

bool EmoticonsProvider::removeEmoticon(const QStringList &codes)
{
	Q_UNUSED(codes);
	return false;
}

void EmoticonsProvider::clearEmoticons()
{
	p->order.clear();
	p->map.clear();
	p->indexes.clear();
}

inline void appendEmoticonToHash(QList<EmoticonsProvider::Emoticon> &ls, const EmoticonsProvider::Emoticon &e)
{ ls.insert(qLowerBound(ls.begin(), ls.end(), e), e); }

void EmoticonsProvider::appendEmoticon(const QString &imgPath, const QStringList &codes)
{
	if (codes.isEmpty())
		return;
	QImageReader reader(imgPath);
	QSize size = reader.size();
	if (!size.isValid()) {
		size = reader.read().size();
		if (!size.isValid())
			return;
	}
	p->order.append(imgPath);
	p->map.insert(imgPath, codes);
	QString imgHtml = QLatin1Literal("<img src=\"")
			% imgPath
			% QLatin1Literal("\" width=\"")
			% QString::number(size.width())
			% QLatin1Literal("\" height=\"")
			% QString::number(size.height())
			% QLatin1Literal("\" alt=\"%4\" title=\"%4\"><span style=\"font-size: 0\">%4</span></img>");
	foreach (const QString &code, codes) {
		Emoticon e;
		e.picPath = imgPath;
		e.picHTMLCode = imgHtml;
		e.matchText = code.toLower();
		e.matchTextEscaped = code.toHtmlEscaped().toLower();
		if (e.matchText.isEmpty() || e.matchTextEscaped.isEmpty())
			continue;
		QChar c1 = code.at(0);
		QChar c2 = e.matchTextEscaped.at(0);
		appendEmoticonToHash(p->indexes[c1], e);
		if (c1 != c2)
			appendEmoticonToHash(p->indexes[c2], e);
	}
}

void EmoticonsProvider::removeEmoticon(const QString &imgPath, const QStringList &codes)
{
	p->order.removeAll(imgPath);
	p->map.remove(imgPath);
	foreach (const QString &code, codes) {
		QString escaped = code.toHtmlEscaped();
		if (code.isEmpty() || escaped.isEmpty())
			continue;
		QHash<QChar, QList<Emoticon> >::iterator it;

		it = p->indexes.find(code.at(0));
		if (it != p->indexes.end()) {
			QList<Emoticon> &ls = it.value();
			for (int i = 0; i < ls.size(); ++i) {
				if (ls.at(i).picPath == imgPath) {
					ls.removeAt(i);
				}
			}
		}

		it = p->indexes.find(escaped.at(0));
		if (it != p->indexes.end()) {
			QList<Emoticon> &ls = it.value();
			for (int i = 0; i < ls.size(); ++i) {
				if (ls.at(i).picPath == imgPath) {
					ls.removeAt(i);
				}
			}
		}
	}
}

EmoticonsTheme::EmoticonsTheme(const QString &name) : p(Emoticons::theme(name).p)
{
}

EmoticonsTheme::EmoticonsTheme(EmoticonsThemeData *data) :
	p(data)
{
}

EmoticonsTheme::EmoticonsTheme(const EmoticonsTheme &other) : p(other.p)
{
}

EmoticonsTheme::~EmoticonsTheme()
{
}

EmoticonsTheme &EmoticonsTheme::operator =(const EmoticonsTheme &other)
{
	p = other.p;
	return *this;
}

bool EmoticonsTheme::isNull() const
{
	return !p || !p->provider;
}

QHash<QString, QStringList> EmoticonsTheme::emoticonsMap() const
{
	return isNull() ? QHash<QString, QStringList>() : p->provider->emoticonsMap();
}

QStringList EmoticonsTheme::emoticonsIndexes() const
{
	return isNull() ? QStringList() : p->provider->emoticonsIndexes();
}

QString EmoticonsTheme::themeName() const
{
	return isNull() ? QString(nullThemeName) : p->provider->themeName();
}

//	EmoticonsTheme EmoticonsTheme::pseudoClone()
//	{
//		return EmoticonsTheme(new PseudoEmoticonsProvider(p->provider));
//	}

QString EmoticonsTheme::parseEmoticons(const QString &text, ParseMode mode, const QStringList &exclude)
{
	if (isNull())
		return text;
	QString result;
	QList<Token> tokens = tokenize(text, mode);
	for (QList<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
		switch(it->type) {
		case Image:
			if (!exclude.contains(it->text)) {
				result += it->imgHtmlCode;
				break;
			}
		case Text:
			result += it->text;
		default:
			break;
		}
	}
	return result;
}

enum HtmlState
{
	OutsideHtml = 0,
	FirstTag,
	TagText,
	SecondTag
};

inline bool compareEmoticon(const QChar *c, const QString &smile)
{
	const QChar *s = smile.constData();
	while (c->toLower() == *s) {
		if (s->isNull())
			return true;
		s++;
		c++;
	}
	return s->isNull();
}

inline void appendEmoticon(QString &text, const QString &url, const QStringRef &emo)
{
	int i = 0, last = 0;
	while ((i = url.indexOf(QLatin1String("%4"), last)) != -1) {
		text += QStringRef(&url, last, i - last);
		text += emo;
		last = i + 2;
	}
	text += QStringRef(&url, last, url.length() - last);
}

QList<EmoticonsTheme::Token> EmoticonsTheme::tokenize(const QString &message, ParseMode mode)
{
	QList<Token> tokens;
	if (isNull()) {
		tokens << Token(message);
		return tokens;
	}
	HtmlState state = OutsideHtml;
	bool at_amp = false;
	const QChar *begin = message.constData();
	const QChar *chars = message.constData();
	QChar cur;
	QString text;
	typedef EmoticonsProvider::Emoticon Emoticon;
	const QHash<QChar, QList<Emoticon> > allEmoticons = p->provider->emoticonsByChar();
	QList<Emoticon>::const_iterator it;
	while (!chars->isNull()) {
		cur = *chars;
		if (cur == '<') {
			if (state == OutsideHtml)
				state = FirstTag;
			else
				state = SecondTag;
		} else if (state == FirstTag || state == SecondTag) {
			switch(cur.unicode()) {
			case L'/':
				state = SecondTag;
				break;
			case L'"':
			case L'\'':
				do text += *(chars++);
				while(!chars->isNull() && *chars != cur);
				break;
			case L'>':
				state = static_cast<HtmlState>((state + 1) % 4);
				break;
			default:
				break;
			}
		} else if (state != TagText && at_amp) {
			do text += *(chars++);
			while(!chars->isNull() && *chars != ';');
			cur = *chars;
			at_amp = false;
		} else if (state != TagText) {
			bool found = false;
			at_amp = cur == '&';
			if (!(mode & StrictParse) || chars == begin || (chars-1)->isSpace()) {
				const QList<Emoticon> emoticons = allEmoticons.value(cur.toLower());
				for (it = emoticons.constBegin(); it != emoticons.constEnd(); it++ ) {
					const Emoticon &emo = *it;
					int length = emo.matchTextEscaped.length();
					if (compareEmoticon(chars, emo.matchTextEscaped)
							&& (!(mode&StrictParse) || (chars+length)->isNull() || (chars+length)->isSpace())) {
						if (!text.isEmpty()) {
							tokens << Token(text);
							text = QString();
						}
						QString htmlCode;
						appendEmoticon(htmlCode, emo.picHTMLCode, QStringRef(&message, chars - begin, length));
						tokens << Token(QString(chars, length), emo.picPath, htmlCode);
						found = true;
						at_amp = false;
						chars += length;
						cur = *chars;
						break;
					}
				}
				if (found)
					continue;
			}
		}
		if (cur.isNull())
			break;
		text += cur;
		chars++;
	}
	if (!text.isEmpty())
		tokens << Token(text);

	return tokens;
}

namespace Emoticons
{
Q_GLOBAL_STATIC_WITH_ARGS(EmoticonsTheme, currentTheme, (0))

EmoticonsTheme theme()
{
	if (currentTheme()->isNull())
		*currentTheme() = theme(QString());
	return *currentTheme();
}

QString currentThemeName()
{
	ensurePrivate();
	if (!currentTheme()->isNull())
		return currentTheme()->themeName();
	ConfigGroup config = Config("appearance").group("emoticons");
	QString name = config.value<QString>("theme", QString());
	if (name.isEmpty()) {
		QStringList themes = themeList();
		if (themes.isEmpty() || themes.contains(QLatin1String("default")))
			name = QLatin1String("default");
		else
			name = themes.first();
		config.setValue("theme", name);
		config.sync();
	}
	return name;
	//			return Config().group("emoticons").value<QString>("theme", "default");
}

EmoticonsTheme theme(const QString &name)
{
	if (!name.isNull() && name == nullThemeName)
		return EmoticonsTheme(0);

	if (name.isEmpty()) {
		QString currentName = currentThemeName();
		if (currentName.isEmpty())
			return EmoticonsTheme(0);
		else
			return theme(currentName);
	}
	else
		ensurePrivate();

	// Firstly look at cache
	if (EmoticonsThemeData *data = p->cache.value(name))
		return EmoticonsTheme(data);

	// Then try a chance in different backends
	foreach (EmoticonsBackend *backend, p->backends) {
		if (backend->themeList().contains(name)) {
			EmoticonsThemeData *data = new EmoticonsThemeData;
			data->provider = backend->loadTheme(name);
			Q_ASSERT(data->provider);
			Q_ASSERT(data->provider->themeName() == name);
			p->cache.insert(name, data);
			return EmoticonsTheme(data);
		}
	}

	// So.. there is no such theme... create null one
	return EmoticonsTheme(0);
}

QStringList themeList()
{
	ensurePrivate();
	QSet<QString> themes;
	foreach (EmoticonsBackend *backend, p->backends) {
		foreach (const QString &theme, backend->themeList())
			themes << theme;
	}
	QStringList result = themes.toList();
	result.prepend(nullThemeName);
	return result;
}

void setTheme(const QString &name)
{
	ConfigGroup group = Config("appearance").group("emoticons");
	group.setValue("theme", name);
	group.sync();
	*currentTheme() = EmoticonsTheme(0);
}

void setTheme(const EmoticonsTheme &theme)
{
	ConfigGroup group = Config("appearance").group("emoticons");
	group.setValue("theme", theme.themeName());
	group.sync();
	*currentTheme() = theme;
}
}
}

