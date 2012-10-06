#include "macspellchecker.h"
#include "macspellsettings.h"
#include <QStringList>
#include <Foundation/NSArray.h>
#include <Foundation/NSString.h>
#include <Foundation/NSAutoreleasePool.h>
#include <AppKit/NSSpellChecker.h>
#include <qutim/config.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

namespace MacIntegration
{
MacSpellChecker *MacSpellChecker::self = 0;

MacSpellChecker::MacSpellChecker()
{
	Q_ASSERT(!self);
	self = this;
	Settings::registerItem(new GeneralSettingsItem<MacSpellSettings>(Settings::General, Icon("tools-check-spelling"),
																	 QT_TRANSLATE_NOOP("Settings", "Spell checker")));
	QString lang = Config().group("speller").value("language", QString());
	if (lang == QLatin1String("system"))
		lang.clear();
	loadSettings(lang);
}

MacSpellChecker::~MacSpellChecker()
{
}

bool MacSpellChecker::isCorrect(const QString &word) const
{
	NSString *mac_word = [[NSString alloc] initWithUTF8String:word.toUtf8().constData()];
	NSRange r = {0, 0};
	r = [[NSSpellChecker sharedSpellChecker] checkSpellingOfString:mac_word startingAt:0];
	int i = r.location;
	[mac_word release];
	if (i == -1)
		return true;
	else
		return false;
}

static QString mac_NSString_to_QString(NSString *string)
{
	NSRange range = NSMakeRange(0, [string length]);
	QScopedArrayPointer<unichar> chars(new unichar[range.length + 1]);
	[string getCharacters: chars.data() range: range];
	return QString::fromUtf16(chars.data(), range.length);
}

QStringList MacSpellChecker::suggest(const QString &word) const
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* mac_word = [[NSString alloc] initWithUTF8String:word.toUtf8().constData()];
	NSRange range = {0, word.length()};
	NSArray* const array = [[NSSpellChecker sharedSpellChecker] guessesForWordRange:range
		inString:mac_word
		language:nil
		inSpellDocumentWithTag:0];
	QStringList result;
	if (array) {
		unsigned int count = [array count];
		for (unsigned int i = 0; i < count; i++) {
			NSString *suggest_word = [array objectAtIndex:i];
			result << mac_NSString_to_QString(suggest_word);
		}
	}
	[pool release];
	return result;
}

void MacSpellChecker::store(const QString &word) const
{
	NSString* mac_word = [[NSString alloc] initWithUTF8String:word.toUtf8().constData()];
	[[NSSpellChecker sharedSpellChecker] learnWord:mac_word];
	[mac_word release];
}

void MacSpellChecker::storeReplacement(const QString &bad, const QString &good)
{
	NSString* mac_good_word = [[NSString alloc] initWithUTF8String:good.toUtf8().constData()];
	NSString* mac_bad_word = [[NSString alloc] initWithUTF8String:bad.toUtf8().constData()];
	[[NSSpellChecker sharedSpellChecker] unlearnWord:mac_bad_word];
	[[NSSpellChecker sharedSpellChecker] learnWord:mac_good_word];
	[mac_good_word release];
	[mac_bad_word release];
}

QStringList MacSpellChecker::languages()
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSArray* const array = [[NSSpellChecker sharedSpellChecker] availableLanguages];
	QStringList result;
	if (array) {
		unsigned int count = [array count];
		for (unsigned int i = 0; i < count; i++) {
			NSRange res_range;
			NSString *suggest_word = [array objectAtIndex:i];
			res_range.location = 0;
			res_range.length = [suggest_word length];
			unichar *chars = new unichar[res_range.length];
			[suggest_word getCharacters:chars range:res_range];
			QString str = QString::fromUtf16(chars, res_range.length);
			delete[] chars;
			result.append(str);
		}
	}
	[pool release];
	return result;
}

QString MacSpellChecker::toPrettyLanguageName(const QString &lang)
{
	QString localeName = lang.mid(0, lang.indexOf('-'));
	QString type = lang.mid(localeName.length()+1);
	QLocale locale(localeName);
	QString name = QString("%1 / %2")
			.arg(QLocale::languageToString(locale.language()))
			.arg(QLocale::countryToString(locale.country()));
	if (!type.isEmpty())
		name += " (" + type + ")";
	return name;
}

void MacSpellChecker::loadSettings(const QString &lang)
{
	NSString* mac_lang = [[NSString alloc] initWithUTF8String:lang.toUtf8().constData()];
	[[NSSpellChecker sharedSpellChecker] setLanguage:mac_lang];
	[mac_lang release];
	emit dictionaryChanged();
}
}
