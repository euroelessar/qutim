/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "emoticonsselector.h"
#include "ui_emoticonsselector.h"
#include <qutim/emoticons.h>
#include <qmovie.h>
#include <QLabel>
#include <flowlayout.h>
#include <QDebug>

EmoticonsSelector::EmoticonsSelector() : ui(new Ui::emoticonsSelector)
{
	ui->setupUi(this);
	connect(ui->themeSelector,SIGNAL(currentIndexChanged(QString)),SLOT(currentIndexChanged(QString)));
	FlowLayout *flowLayout = new FlowLayout;
	ui->emoticons->setLayout(flowLayout);
}

EmoticonsSelector::~EmoticonsSelector()
{
	delete ui;
	clearEmoticonsPreview();
}

void EmoticonsSelector::loadImpl()
{
	 QStringList themeList =  Emoticons::themeList();
	 int index = themeList.indexOf(QLatin1String(""));
	 themeList.removeAt(index);
	 themeList.sort();
	 themeList.prepend(tr("No emoticons"));
	 ui->themeSelector->addItems(themeList);
	 cancelImpl();
}

void EmoticonsSelector::cancelImpl()
{
	int index = ui->themeSelector->findText(Emoticons::currentThemeName());
	ui->themeSelector->setCurrentIndex(qMax(0, index));
}

void EmoticonsSelector::saveImpl()
{
	Emoticons::setTheme(m_selected_theme);
}

void EmoticonsSelector::currentIndexChanged(const QString& text)
{
	clearEmoticonsPreview();
	const QStringList emoticons = Emoticons::theme(text).emoticonsIndexes();
	for (int i = 0; i < emoticons.size(); ++i) {
		QLabel *label = new QLabel(this);
		QMovie *emoticon = new QMovie (emoticons.at(i), QByteArray(), label);
		label->setMovie(emoticon);
		ui->emoticons->layout()->addWidget(label);
		m_active_emoticons.append(label);
		emoticon->start();
	}
	m_selected_theme = text;
	emit modifiedChanged(true);
}

void EmoticonsSelector::clearEmoticonsPreview()
{
	qDeleteAll(m_active_emoticons);
	m_active_emoticons.clear();
}

