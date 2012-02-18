/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "kdespellersettings.h"
#include "kdespellerlayer.h"
#include "ui_kdespellersettings.h"
#include <qutim/configbase.h>

KdeSpellerSettings::KdeSpellerSettings() :
    m_ui(new Ui::KdeSpellerSettings)
{
	m_ui->setupUi(this);
	lookForWidgetState(m_ui->dictionaryComboBox);
	lookForWidgetState(m_ui->autodetect);
}

KdeSpellerSettings::~KdeSpellerSettings()
{
    delete m_ui;
}

QString KdeSpellerSettings::suggestLanguage(QString lang, Speller *speller)
{
	QStringList langs = speller->availableLanguages();
	if (langs.contains(lang))
		return lang;
	lang = QLocale::system().name();
	if (langs.contains(lang))
		return lang;
	lang = lang.section('_', 0, 0);
	if (langs.contains(lang))
		return lang;
	return QString();
}

void KdeSpellerSettings::loadImpl()
{
	Speller *speller = KdeSpellerLayer::spellerInstance();
	ConfigGroup group = Config().group("speller");
	m_ui->autodetect->setChecked(group.value("autodetect", false));
	QString lang = suggestLanguage(group.value("language", QString()), speller);
	if (!lang.isEmpty())
		m_ui->dictionaryComboBox->setCurrentByDictionary(lang);
}

void KdeSpellerSettings::saveImpl()
{
	QString lang = m_ui->dictionaryComboBox->currentDictionary();
	ConfigGroup group = Config().group("speller");
	group.setValue("autodetect", m_ui->autodetect->isChecked());
	group.setValue("language", lang);
	KdeSpellerLayer::spellerInstance()->setLanguage(lang);
}

void KdeSpellerSettings::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

