#include "emoticons.h"
#include <QStringList>

namespace qutim_sdk_0_3
{
	struct EmoticonsThemeData : public QSharedData
	{
		EmoticonsProvider *provider;
	};

	EmoticonsProvider::EmoticonsProvider()
	{
	}

	EmoticonsProvider::~EmoticonsProvider()
	{
	}

	EmoticonsTheme::EmoticonsTheme(EmoticonsProvider *provider)
	{
	}

	EmoticonsTheme::EmoticonsTheme(const EmoticonsTheme &theme)
	{
	}

	EmoticonsTheme::~EmoticonsTheme()
	{
	}

	bool EmoticonsTheme::isNull()
	{
		return p->provider ? false : true;
	}

	QString EmoticonsTheme::parseEmoticons(const QString &text, ParseMode mode, const QStringList &exclude)
	{
		QString result;
		QList<Token> tokens = tokenize(text, mode);
		for(QList<Token>::iterator it = tokens.begin(); it != tokens.end(); it++)
		{
			Token *it = it;
			switch(it->type)
			{
			case Image:
				if(!exclude.contains(it->text)) {
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

	bool inline compareEmoticon(const QChar *c, const QString &smile)
	{
		const QChar *s = smile.constData();
		while(c->toLower() == *s)
		{
			if(s->isNull())
				return true;
			s++;
			c++;
		}
		return s->isNull();
	}

	QList<EmoticonsTheme::Token> EmoticonsTheme::tokenize(const QString &message, ParseMode mode)
	{
		QList<Token> tokens;
		HtmlState state = OutsideHtml;
		bool at_amp = false;
		const QChar *begin = message.constData();
		const QChar *chars = message.constData();
		QChar cur;
		QString text;
//		QList<QPair<QString,QString> >::const_iterator it;
		while(!chars->isNull())
		{
			cur = *chars;
			if(cur == '<')
			{
				if(state == OutsideHtml)
					state = FirstTag;
				else
					state = SecondTag;
			}
			else if(state == FirstTag || state == SecondTag)
			{
				switch(cur.unicode())
				{
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
			}
			else if(state != TagText && at_amp)
			{
				do text += *(chars++);
				while(!chars->isNull() && *chars != ';');
				cur = *chars;
				at_amp = false;
			}
			else if(state != TagText)
			{
				bool found = false;
				at_amp = cur == '&';
				if(!(mode & StrictParse) || chars == begin || (chars-1)->isSpace())
				{
				/*	it = m_emoticons.constBegin();
					for( ; it != m_emoticons.constEnd(); it++ )
					{
						int length = (*it).first.length();
						if(compareEmoticon(chars, (*it).first)
							&& (!m_check_space || (chars+length)->isNull() || (chars+length)->isSpace()))
						{
							if(!text.isEmpty())
							{
								tokens << Token(text);
								text = QString();
							}
							tokens << Token(text, path, html);
							appendEmoticon(text, (*it).second, QStringRef(&message, chars - begin, (*it).first.length()));
							found = true;
							at_amp = false;
							chars += length;
							cur = *chars;
							break;
						}
					}
					if(found)
						continue;*/
				}
			}
			if(cur.isNull())
				break;
			text += cur;
			chars++;
		}
		if(!text.isEmpty())
			tokens << Token(text);

		return tokens;
	}
}
