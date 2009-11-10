/****************************************************************************
 *  chatappearance.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include "chatappearance.h"
#include "ui_chatappearance.h"
#include "chatstyle.h"
#include "chatstyleoutput.h"
#include <QDebug>
#include <libqutim/configbase.h>

namespace AdiumChat
{
	
	ChatAppearance::ChatAppearance(): ui(new Ui::chatAppearance), m_chat_style_output(new ChatStyleOutput), m_page(new QWebPage)
	{
		ui->setupUi(this);
		connect(ui->chatBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
	}
	
	ChatAppearance::~ChatAppearance()
	{
		delete ui;
	}
	
	void ChatAppearance::cancelImpl()
	{

	}

	void ChatAppearance::loadImpl()
	{
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		m_current_style_name = adium_chat.value<QString>("name","default");
		m_current_variant = adium_chat.value<QString>("variant", QString());
		getThemes();
		//TODO need fake preview acc
		//m_chat_style_output->preparePage(m_page,acc,"themePreview");
	}

	void ChatAppearance::saveImpl()
	{
		m_current_style_name = ui->chatBox->itemData(ui->chatBox->currentIndex()).toMap().value("name").toString();
		m_current_style_name = ui->chatBox->itemData(ui->chatBox->currentIndex()).toMap().value("variant").toString();
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("style");
		adium_chat.setValue("name",m_current_style_name);
		adium_chat.setValue("variant",m_current_variant);
		adium_chat.sync();
	}

	void ChatAppearance::getThemes()
	{
		QString category = "webkitstyle";
		int default_index = -1;
		bool index_found = false;
		QStringList themes = listThemes(category);
		foreach (QString name, themes)
		{
			//FIXME optimize!
			StyleVariants variants = ChatStyleGenerator::listVariants(getThemePath(name,category).append("/Contents/Resources/Variants"));
			QVariantMap data;
			data["name"] = name;
 			if (variants.isEmpty())
			{
				ui->chatBox->addItem(name,data);
				if (!index_found && name == m_current_style_name)
				{
					index_found = true;
					default_index = ui->chatBox->count() - 1;
				}
			}
			else
			{
				StyleVariants::const_iterator it;
				for (it=variants.begin();it!=variants.end();it++)
				{
					data["variant"] = it.key();
					ui->chatBox->addItem(tr("%1 (%2)").arg(name).arg(it.key()),data);
					if (!index_found && name == m_current_style_name && it.key() == m_current_variant)
					{
						index_found = true;
						default_index = ui->chatBox->count() - 1;
					}
				}
			}
		}
		qDebug() << default_index << m_current_style_name << m_current_variant;
		ui->chatBox->setCurrentIndex(default_index);
	}
	
	void ChatAppearance::onCurrentIndexChanged(int index)
	{
		QVariantMap map = ui->chatBox->itemData(index).toMap();
	}

}