#include "mspellsettings.h"
#include "mspellchecker.h"
#include <QComboBox>
#include <QFormLayout>
#include <qutim/config.h>


namespace MacIntegration
{
	MSpellSettings::MSpellSettings()
	{
		QFormLayout *layout = new QFormLayout(this);
		m_languagesBox = new QComboBox(this);
		layout->addRow(tr("Language"), m_languagesBox);
		lookForWidgetState(m_languagesBox);
	}

	MSpellSettings::~MSpellSettings()
	{
	}

	void MSpellSettings::loadImpl()
	{
		m_languagesBox->clear();
		m_languagesBox->addItem("System", "system");
		int current = -1, i = 0;
		Config group = Config().group("speller");
		QString currentLang = group.value("language", QString());
		foreach (const QString &lang, MSpellChecker::instance()->languages()) {
			m_languagesBox->addItem(MSpellChecker::toPrettyLanguageName(lang), lang);
			if (current == -1 && lang == currentLang)
				current = i;
			++i;
		}
		++current;
		m_languagesBox->setCurrentIndex(current);
	}

	void MSpellSettings::saveImpl()
	{
		QString lang = m_languagesBox->itemData(m_languagesBox->currentIndex()).toString();
		Config group = Config().group("speller");
		group.setValue("language", lang);
		MSpellChecker::instance()->loadSettings(lang);
	}
}
