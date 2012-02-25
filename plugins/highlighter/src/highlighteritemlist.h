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

#ifndef HIGHLIGHTERITEMLIST_H
#define HIGHLIGHTERITEMLIST_H

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class HighlighterItemList : public QWidget
{
	Q_OBJECT

public:
	typedef QSharedPointer<HighlighterItemList> Guard;
	
	HighlighterItemList(const QRegExp &regex, QListWidget *regexList);
	~HighlighterItemList();
	
	QRegExp regexp() const;

	QListWidgetItem *item();
	void setItem(QListWidgetItem *item);

	QString getTranslatedRegexpType(const QRegExp::PatternSyntax &syntax);

signals:
	void buttonClicked();

private:
	QLabel *m_label;
	QPushButton *m_button;
	QListWidgetItem *m_item;
	QRegExp m_regexp;
};

#endif // HIGHLIGHTERITEMLIST_H

