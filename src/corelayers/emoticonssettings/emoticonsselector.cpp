/****************************************************************************
 *  emoticonsselector.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "emoticonsselector.h"
#include "ui_emoticonsselector.h"
#include <libqutim/emoticons.h>
#include <qmovie.h>
#include <QLabel>
#include "flowlayout.h"

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
	 QStringList theme_list =  Emoticons::themeList();
	 ui->themeSelector->addItems(theme_list);
	 int index = ui->themeSelector->findText(Emoticons::currentThemeName());
	 ui->themeSelector->setCurrentIndex(index);
}

void EmoticonsSelector::cancelImpl()
{

}

void EmoticonsSelector::saveImpl()
{
	Emoticons::setTheme(m_selected_theme);
}

void EmoticonsSelector::currentIndexChanged(const QString& text)
{
	QHash<QString, QStringList> theme_map = Emoticons::theme(text).emoticonsMap();
	QHash<QString, QStringList>::const_iterator it;
	clearEmoticonsPreview();
	for (it = theme_map.constBegin();it != theme_map.constEnd();it ++) {
		QLabel *label = new QLabel();
		QMovie *emoticon = new QMovie (it.key(), QByteArray(), label);
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
	qDeleteAllLater(m_active_emoticons);
	m_active_emoticons.clear();
}
