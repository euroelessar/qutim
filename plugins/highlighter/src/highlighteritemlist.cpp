/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#include "highlighteritemlist.h"
#include <QTextDocument>
#include <qutim/icon.h>
#include <QRegExp>

Q_DECLARE_METATYPE(HighlighterItemList::Guard)

HighlighterItemList::HighlighterItemList(const QRegExp &regex, QListWidget *regexList)
	: m_regexp(regex)
{

	QString title = QString::fromLatin1("%1<br>%2")
			.arg(Qt::escape(regex.pattern()), getTranslatedRegexpType(regex.patternSyntax()));
	m_label = new QLabel(title, this);
	QSizePolicy policy = m_label->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	m_label->setSizePolicy(policy);
	m_button = new QPushButton(tr("Remove"), this);
	m_button->setIcon(qutim_sdk_0_3::Icon(QLatin1String("list-remove")));
	connect(m_button, SIGNAL(clicked(bool)), this, SIGNAL(buttonClicked()));
	
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(m_label);
	layout->addWidget(m_button);

	m_item = new QListWidgetItem(regexList);
	m_item->setData(Qt::UserRole, qVariantFromValue(Guard(this)));
	m_item->setData(Qt::SizeHintRole, sizeHint());
	regexList->setItemWidget(m_item, this);
}

HighlighterItemList::~HighlighterItemList()
{
}

QRegExp HighlighterItemList::regexp() const
{
	return m_regexp;
}

QListWidgetItem *HighlighterItemList::item()
{
	return m_item;
}

void HighlighterItemList::setItem(QListWidgetItem *item)
{
	m_item = item;
}

QString HighlighterItemList::getTranslatedRegexpType(const QRegExp::PatternSyntax &syntax)
{
	switch(syntax)
	{
	case QRegExp::RegExp:
		return tr("Perl-like");
	case QRegExp::RegExp2:
		return tr("Improved perl-like");
	case QRegExp::Wildcard:
		return tr("Shell-like");
	case QRegExp::WildcardUnix:
		return tr("Shell-like Unix");
	case QRegExp::FixedString:
		return tr("Fixed string");
	case QRegExp::W3CXmlSchema11:
		return tr("W3C XML Schema");
	default:
		return tr("Perl-like");
	}
}

void HighlighterItemList::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		m_label->setText(getTranslatedRegexpType(regexp().patternSyntax()));
		break;
	default:
		break;
	}
}
