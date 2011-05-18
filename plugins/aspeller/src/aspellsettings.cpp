#include "aspellsettings.h"
#include "aspellchecker.h"
#include <QComboBox>
#include <QFormLayout>
#include <qutim/config.h>

AspellSettings::AspellSettings()
{
	QFormLayout *layout = new QFormLayout(this);
	m_languagesBox = new QComboBox(this);
	layout->addRow(tr("Language"), m_languagesBox);
	lookForWidgetState(m_languagesBox);
}

AspellSettings::~AspellSettings()
{
}

void AspellSettings::loadImpl()
{
	m_languagesBox->clear();
	m_languagesBox->addItem("System", "system");
	int current = -1, i = 0;
	Config group = Config().group("speller");
	QString currentLang = group.value("language", QString());
	foreach (const QString &lang, ASpellChecker::instance()->languages()) {
		m_languagesBox->addItem(ASpellChecker::toPrettyLanguageName(lang), lang);
		if (current == -1 && lang == currentLang)
			current = i;
		++i;
	}
	++current;
	m_languagesBox->setCurrentIndex(current);
}

void AspellSettings::saveImpl()
{
	QString lang = m_languagesBox->itemData(m_languagesBox->currentIndex()).toString();
	Config group = Config().group("speller");
	group.setValue("language", lang);
	ASpellChecker::instance()->loadSettings(lang);
}
