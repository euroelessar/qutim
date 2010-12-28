#include "hunspellsettings.h"
#include "hunspellchecker.h"
#include <QComboBox>
#include <QFormLayout>
#include <qutim/config.h>

HunSpellSettings::HunSpellSettings()
{
	QFormLayout *layout = new QFormLayout(this);
	m_languagesBox = new QComboBox(this);
	layout->addRow(tr("Language"), m_languagesBox);
	lookForWidgetState(m_languagesBox);
}

HunSpellSettings::~HunSpellSettings()
{
}

void HunSpellSettings::loadImpl()
{
	m_languagesBox->clear();
	m_languagesBox->addItem("System", "system");
	int current = -1, i = 0;
	Config group = Config().group("speller");
	QString currentLang = group.value("language", QString());
	foreach (const QString &lang, HunSpellChecker::instance()->languages()) {
		m_languagesBox->addItem(HunSpellChecker::toPrettyLanguageName(lang), lang);
		if (current == -1 && lang == currentLang)
			current = i;
		++i;
	}
	++current;
	m_languagesBox->setCurrentIndex(current);
}

void HunSpellSettings::saveImpl()
{
	QString lang = m_languagesBox->itemData(m_languagesBox->currentIndex()).toString();
	Config group = Config().group("speller");
	group.setValue("language", lang);
	HunSpellChecker::instance()->loadSettings(lang);
}
