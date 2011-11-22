/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "macspellsettings.h"
#include "macspellchecker.h"
#include <QComboBox>
#include <QFormLayout>
#include <qutim/config.h>


namespace MacIntegration
{
MacSpellSettings::MacSpellSettings()
{
	QFormLayout *layout = new QFormLayout(this);
	m_languagesBox = new QComboBox(this);
	layout->addRow(tr("Language"), m_languagesBox);
	lookForWidgetState(m_languagesBox);
}

MacSpellSettings::~MacSpellSettings()
{
}

void MacSpellSettings::loadImpl()
{
	m_languagesBox->clear();
	m_languagesBox->addItem("System", "system");
	int current = -1, i = 0;
	Config group = Config().group("speller");
	QString currentLang = group.value("language", QString());
	foreach (const QString &lang, MacSpellChecker::instance()->languages()) {
		m_languagesBox->addItem(MacSpellChecker::toPrettyLanguageName(lang), lang);
		if (current == -1 && lang == currentLang)
			current = i;
		++i;
	}
	++current;
	m_languagesBox->setCurrentIndex(current);
}

void MacSpellSettings::saveImpl()
{
	QString lang = m_languagesBox->itemData(m_languagesBox->currentIndex()).toString();
	Config group = Config().group("speller");
	group.setValue("language", lang);
	MacSpellChecker::instance()->loadSettings(lang);
}
}

