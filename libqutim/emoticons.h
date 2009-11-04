#ifndef EMOTICONS_H
#define EMOTICONS_H

#include "libqutim_global.h"
#include <QSharedData>
#include <QStringList>

namespace qutim_sdk_0_3
{
	class EmoticonsProvider;
	struct EmoticonsThemeData;

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
			inline Token(const QString &p, const QString &h) : type(Image), imgPath(p), imgHtmlCode(h) {}
			TokenType type;
			QString text;
			QString imgPath;
			QString imgHtmlCode;
		};

		EmoticonsTheme(EmoticonsProvider *provider);
		EmoticonsTheme(const EmoticonsTheme &theme);
		~EmoticonsTheme();

		bool isNull();

		QString parseEmoticons(const QString &text, ParseMode mode = DefaultParse, const QStringList &exclude = QStringList());
		QList<Token> tokenize(const QString &text, ParseMode mode = DefaultParse);
	private:
		QExplicitlySharedDataPointer<EmoticonsThemeData> p;
	};

	class LIBQUTIM_EXPORT EmoticonsProvider
	{
	public:
		struct Emoticon
		{
		};
		EmoticonsProvider();
		virtual ~EmoticonsProvider();
		virtual QHash<QString, QStringList> emoticonsMap() = 0;
		virtual QStringList emoticonsIndexes() = 0;
		virtual QHash<QChar, QList<Emoticon> > emoticonsByChar() = 0;
	};

	class LIBQUTIM_EXPORT EmoticonsBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual QStringList themeList() = 0;
		virtual EmoticonsTheme loadTheme(const QString &name) = 0;
		virtual EmoticonsTheme saveTheme(const EmoticonsTheme &theme) = 0;
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
