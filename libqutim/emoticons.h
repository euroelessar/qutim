/****************************************************************************
 *  emoticons.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef EMOTICONS_H
#define EMOTICONS_H

#include "libqutim_global.h"
#include <QSharedData>
#include <QStringList>

namespace qutim_sdk_0_3
{
	class EmoticonsProvider;
	struct EmoticonsThemeData;
	struct EmoticonsProviderPrivate;

	class LIBQUTIM_EXPORT EmoticonsTheme
	{
	public:
		enum ParseModeFlag
		{
			DefaultParse = 0,
			StrictParse  = 0x01
		};
		Q_DECLARE_FLAGS(ParseMode, ParseModeFlag)

		enum TokenType
		{
			Undefined,
			Text,
			Image
		};

		struct Token
		{
			inline Token() : type(Undefined) {}
			inline Token(const QString &t) : type(Text), text(t) {}
			inline Token(const QString &t, const QString &p, const QString &h)
					: type(Image), text(t), imgPath(p), imgHtmlCode(h) {}
			TokenType type;
			QString text;
			QString imgPath;
			QString imgHtmlCode;
		};

		EmoticonsTheme(const QString &name = QString());
		EmoticonsTheme(EmoticonsThemeData *data);
		EmoticonsTheme(const EmoticonsTheme &theme);
		~EmoticonsTheme();
		EmoticonsTheme &operator =(const EmoticonsTheme &theme);

		bool isNull() const;

		QHash<QString, QStringList> emoticonsMap() const;
		QStringList emoticonsIndexes() const;
		QString themeName() const;

//		EmoticonsTheme pseudoClone();

		QString parseEmoticons(const QString &text, ParseMode mode = DefaultParse, const QStringList &exclude = QStringList());
		QList<Token> tokenize(const QString &text, ParseMode mode = DefaultParse);
	private:
		QExplicitlySharedDataPointer<EmoticonsThemeData> p;
	};

	class LIBQUTIM_EXPORT EmoticonsProvider
	{
	public:
		// Emoticon has the same structure as in KDE for binary hacks
		struct Emoticon
		{
			Emoticon(){}
			/* sort by longest to shortest matchText */
			bool operator < (const Emoticon &e) const { return matchText.length() > e.matchText.length(); }
			QString matchText;
			QString matchTextEscaped;
			QString picPath;
			QString picHTMLCode;
		};
		EmoticonsProvider();
		virtual ~EmoticonsProvider();
		QHash<QString, QStringList> emoticonsMap() const;
		QStringList emoticonsIndexes() const;
		QHash<QChar, QList<Emoticon> > emoticonsByChar() const;
		virtual QString themeName() const;
		virtual bool saveTheme();
		// Warning: QStringList must contain lower-case strings
		virtual bool addEmoticon(const QString &imgPath, const QStringList &codes);
		virtual bool removeEmoticon(const QStringList &codes);
	protected:
		void clearEmoticons();
		void appendEmoticon(const QString &imgPath, const QStringList &codes);
		void removeEmoticon(const QString &imgPath, const QStringList &codes);
	private:
		QScopedPointer<EmoticonsProviderPrivate> p;
	};

	class LIBQUTIM_EXPORT EmoticonsBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual QStringList themeList() = 0;
		virtual EmoticonsProvider *loadTheme(const QString &name) = 0;
	};

	namespace Emoticons
	{
		LIBQUTIM_EXPORT EmoticonsTheme theme();
		LIBQUTIM_EXPORT QString currentThemeName();
		LIBQUTIM_EXPORT EmoticonsTheme theme(const QString &name);
		LIBQUTIM_EXPORT QStringList themeList();
		LIBQUTIM_EXPORT void setTheme(const QString &name);
		LIBQUTIM_EXPORT void setTheme(const EmoticonsTheme &theme);
	}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(qutim_sdk_0_3::EmoticonsTheme::ParseMode)

#endif // EMOTICONS_H
