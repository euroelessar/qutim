/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "highlightersettings.h"
#include <qutim/config.h>

using namespace qutim_sdk_0_3;

HighlighterSettings::HighlighterSettings()
{
	ui.setupUi(this);
	lookForWidgetState(ui.enableSimpleHighlights);
	lookForWidgetState(ui.pattern);

	connect(ui.addRegexp, SIGNAL(clicked()), this,  SLOT(onAddButtonClicked()));
	connect(ui.regexp, SIGNAL(textChanged(const QString &)), this,  SLOT(validateInputRegexp()));
	connect(ui.regexptype, SIGNAL(currentIndexChanged(int)), this,  SLOT(validateInputRegexp()));


	ui.regexptype->addItem(HighlighterItemList::getTranslatedRegexpType(QRegExp::RegExp), QVariant(QRegExp::RegExp));
	ui.regexptype->addItem(HighlighterItemList::getTranslatedRegexpType(QRegExp::RegExp2), QVariant(QRegExp::RegExp2));
	ui.regexptype->addItem(HighlighterItemList::getTranslatedRegexpType(QRegExp::Wildcard), QVariant(QRegExp::Wildcard));
	ui.regexptype->addItem(HighlighterItemList::getTranslatedRegexpType(QRegExp::WildcardUnix), QVariant(QRegExp::WildcardUnix));
	ui.regexptype->addItem(HighlighterItemList::getTranslatedRegexpType(QRegExp::FixedString), QVariant(QRegExp::FixedString));
	ui.regexptype->addItem(HighlighterItemList::getTranslatedRegexpType(QRegExp::W3CXmlSchema11), QVariant(QRegExp::W3CXmlSchema11));
}

void HighlighterSettings::loadImpl()
{
	Config cfg;

	cfg.beginGroup(QLatin1String("highlighter"));
	ui.pattern->setText(cfg.value("pattern", "\b%nick%\b"));
	ui.enableSimpleHighlights->setChecked(cfg.value("enableSimpleHighlights", true));

	int count = cfg.beginArray(QLatin1String("regexps"));
	for (int i = 0; i < count; i++) {
		cfg.setArrayIndex(i);
		QRegExp regExp = cfg.value(QLatin1String("regexp"), QRegExp());

		HighlighterItemList *item = new HighlighterItemList(regExp, ui.regexpsList);
		connect(item, SIGNAL(buttonClicked()), this, SLOT(onRemoveButtonClicked()));
		m_items << item;
	}
	cfg.endArray();

	cfg.endGroup();
}

void HighlighterSettings::saveImpl()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("highlighter"));
	cfg.setValue("pattern", ui.pattern->text());
	cfg.setValue("enableSimpleHighlights", ui.enableSimpleHighlights->isChecked());

	int count = cfg.beginArray(QLatin1String("regexps"));
	for (int i = 0; i < m_items.size(); i++) {
		cfg.setArrayIndex(i);
		HighlighterItemList *item = m_items.at(i);
		cfg.setValue(QLatin1String("regexp"), item->regexp());
	}
	for (int i = count - 1; i >= m_items.size(); --i)
		cfg.remove(i);
	cfg.endGroup();
}

void HighlighterSettings::cancelImpl()
{
	loadImpl();
}

HighlighterSettings::~HighlighterSettings()
{
//	delete ui;
}

void HighlighterSettings::onRemoveButtonClicked()
{
	HighlighterItemList *item = qobject_cast<HighlighterItemList*>(sender());
	Q_ASSERT(item);
	m_items.removeOne(item);
	delete item->item();
}

void HighlighterSettings::onAddButtonClicked()
{
	int index = ui.regexptype->currentIndex();
	QRegExp regexp(ui.regexp->text());

	regexp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(ui.regexptype->itemData(index).toInt()));
	if (regexp.isEmpty())
		return;

	HighlighterItemList *item = new HighlighterItemList(regexp, ui.regexpsList);
	connect(item, SIGNAL(buttonClicked()), this, SLOT(onRemoveButtonClicked()));
	m_items << item;

	emit modifiedChanged(true);
}

void HighlighterSettings::validateInputRegexp()
{
	int index = ui.regexptype->currentIndex();
	QRegExp regexp(ui.regexp->text());
	regexp.setPatternSyntax(static_cast<QRegExp::PatternSyntax>(ui.regexptype->itemData(index).toInt()));
	if(!regexp.isValid())
	{
		ui.regexp->setStyleSheet(QLatin1String("background: rgb(252, 190, 189);"));
		ui.addRegexp->setDisabled(true);
	}
	else
	{
		ui.regexp->setStyleSheet(QLatin1String("background: #FFF;"));
		ui.addRegexp->setDisabled(false);
	}

}

void HighlighterSettings::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		for(int i = ui.regexptype->count(); i < 0; i--)
		{
			QRegExp::PatternSyntax itemSyntax = static_cast<QRegExp::PatternSyntax>(ui.regexptype->itemData(i).toInt());
			ui.regexptype->setItemText(i, HighlighterItemList::getTranslatedRegexpType(itemSyntax));
		}
		break;
	default:
		break;
	}
}
