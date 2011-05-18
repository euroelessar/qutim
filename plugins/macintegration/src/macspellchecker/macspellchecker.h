#ifndef MACSPELLCHECKER_H
#define MACSPELLCHECKER_H

#include <qutim/spellchecker.h>

namespace MacIntegration
{
using namespace qutim_sdk_0_3;

class MacSpellChecker : public SpellChecker
{
	Q_OBJECT
public:
	MacSpellChecker();
	~MacSpellChecker();
	virtual bool isCorrect(const QString &word) const;
	virtual QStringList suggest(const QString &word) const;
	virtual void store(const QString &word) const;
	virtual void storeReplacement(const QString &bad, const QString &good);
	QStringList languages();
	void loadSettings(const QString &lang);
	static MacSpellChecker *instance() { Q_ASSERT(self); return self; }
	static QString toPrettyLanguageName(const QString &lang);
private:
	static MacSpellChecker *self;
};
}

#endif // MACSPELLCHECKER_H
