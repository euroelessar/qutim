#ifndef MSPELLCHECKER_H
#define MSPELLCHECKER_H

#include <qutim/spellchecker.h>

namespace MacIntegration
{
	using namespace qutim_sdk_0_3;

	class MSpellChecker : public SpellChecker
	{
		Q_OBJECT
		public:
			MSpellChecker();
			~MSpellChecker();
			virtual bool isCorrect(const QString &word) const;
			virtual QStringList suggest(const QString &word) const;
			virtual void store(const QString &word) const;
			virtual void storeReplacement(const QString &bad, const QString &good);
			QStringList languages();
			void loadSettings(const QString &lang);
			static MSpellChecker *instance() { Q_ASSERT(self); return self; }
			static QString toPrettyLanguageName(const QString &lang);
		private:
			static MSpellChecker *self;
	};
}

#endif // MSPELLCHECKER_H
