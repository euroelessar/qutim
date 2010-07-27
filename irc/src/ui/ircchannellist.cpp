/****************************************************************************
 *  ircchannellist.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircchannellist.h"
#include "ui_ircchannellist.h"
#include <ircaccount.h>
#include <ircchannel.h>
#include <qutim/icon.h>
#include <qutim/messagesession.h>

namespace qutim_sdk_0_3 {

namespace irc {

IrcChannelListForm::IrcChannelListForm(IrcAccount *account,QWidget *parent) :
	QWidget(parent), ui(new Ui::IrcChannelListForm), m_account(account)
{
    ui->setupUi(this);
	connect(ui->startButton, SIGNAL(clicked()), SLOT(onStartSearch()));
	connect(ui->filterEdit, SIGNAL(returnPressed()), SLOT(onStartSearch()));
	connect(ui->channelsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
			SLOT(onItemClicked(QTreeWidgetItem*)));
	ui->startButton->setIcon(Icon("media-playback-start"));
}

IrcChannelListForm::~IrcChannelListForm()
{
    delete ui;
}

void IrcChannelListForm::listStarted()
{
	ui->channelsWidget->clear();
	ui->startButton->setEnabled(false);
	ui->filterEdit->setEnabled(false);
	m_count = 0;
	ui->infoLabel->setText(tr("Fetching channels list..."));
}

void IrcChannelListForm::listEnded()
{
	ui->channelsWidget->sortByColumn(0, Qt::AscendingOrder);
	ui->startButton->setEnabled(true);
	ui->filterEdit->setEnabled(true);
	ui->infoLabel->setText(tr("Channels list loaded. (%1)").arg(m_count));
}

void IrcChannelListForm::addChannel(const QString &channel, const QString &users, const QString &topic)
{
	ui->infoLabel->setText(tr("Fetching channels list... (%1)").arg(++m_count));
	QTreeWidgetItem *item = new QTreeWidgetItem(ui->channelsWidget);
	item->setText(0, channel);
	item->setText(1, users);
	item->setText(2, topic);
	ui->channelsWidget->addTopLevelItem(item);
}

void IrcChannelListForm::error(const QString &error)
{
	ui->infoLabel->setText(error);
}

void IrcChannelListForm::onStartSearch()
{
	m_account->send(QString("LIST :%1").arg(ui->filterEdit->text()));

}

void IrcChannelListForm::onItemClicked(QTreeWidgetItem *item)
{
	QString channelName = item->text(0);
	if (!channelName.isEmpty()) {
		IrcChannel *channel = m_account->getChannel(channelName, true);
		channel->join();
		ChatLayer::instance()->getSession(channel, true)->activate();
	}
}

void IrcChannelListForm::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

} } // namespace qutim_sdk_0_3::irc
