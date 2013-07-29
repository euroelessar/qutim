/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "ircchannellist.h"
#include "ui_ircchannellist.h"
#include "../ircaccount.h"
#include "../ircchannel.h"
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <QAbstractTextDocumentLayout>
#include <QPainter>

Q_DECLARE_METATYPE(QTextDocument *)

namespace qutim_sdk_0_3 {

namespace irc {

ChannelsModel::ChannelsModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

ChannelsModel::~ChannelsModel()
{
}

void ChannelsModel::addChannel(const QString &name, const QString &users, const QString &topic)
{
	int pos = 0;
	foreach (const Channel &channel, m_channels) {
		if (QString::localeAwareCompare(name, channel.name) < 0)
			break;
		++pos;
	}
	beginInsertRows(QModelIndex(), pos, pos);
	Channel channel = { name, users, new QTextDocument(this) };
	channel.topic->setHtml(topic);
	m_channels.insert(pos, channel);
	endInsertRows();
}

void ChannelsModel::clear()
{
	int count = m_channels.count();
	beginRemoveRows(QModelIndex(), 0, count);
	m_channels.clear();
	endRemoveRows();
}

int ChannelsModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_channels.count();
}

int ChannelsModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 3;
}

QVariant ChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
		return QVariant();
	switch (section) {
	case 0:
		return tr("Channel");
	case 1:
		return tr("Users");
	case 2:
		return tr("Topic");
	}
	return QVariant();
}

QVariant ChannelsModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if (role == Qt::DisplayRole) {
		Channel channel = m_channels.value(index.row());
		switch(index.column()) {
		case 0:
			return channel.name;
		case 1:
			return channel.users;
		case 2:
			return qVariantFromValue(channel.topic);
		}
	} else if (role == Qt::ToolTipRole) {
		Channel channel = m_channels.value(index.row());
		QString text = QLatin1String("<b>") + channel.name + QLatin1String("</b>");
		QString html = channel.topic->toHtml();
		if (!html.isEmpty())
			text += QLatin1String("</br>") + html;
		return text;
	}
	return QVariant();
}

ChannelsDelegate::ChannelsDelegate(QWidget *parent) :
		QStyledItemDelegate(parent)
{
}

void ChannelsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt,
						  const QModelIndex &index) const
{
	QVariant data = index.data();
	if (data.canConvert<QTextDocument *>()) {
        QTextDocument *textDocument = data.value<QTextDocument *>();
		QStyleOptionViewItemV4 option(opt);
		QStyle *style = option.widget ? option.widget->style() : QApplication::style();
		painter->save();
		// Draw background.
		style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, option.widget);
		// Draw text using QTextDocument.
		int pad = 1;
		QRect textRect = option.rect.adjusted(pad, pad, -pad, -pad);
		QRect clipRect(0, 0, textRect.width(), textRect.height());
		painter->translate(textRect.x(), textRect.y());
		painter->setClipRect(clipRect);
		QAbstractTextDocumentLayout::PaintContext ctx;
		ctx.palette = option.palette;
		ctx.clip = QRect(clipRect);
		textDocument->documentLayout()->draw(painter, ctx);
		painter->restore();
	} else {
		QStyledItemDelegate::paint(painter, opt, index);
	}
}

QSize ChannelsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}

IrcChannelListForm::IrcChannelListForm(IrcAccount *account,QWidget *parent) :
	QWidget(parent),
	ui(new Ui::IrcChannelListForm),
	m_account(account),
	m_model(new ChannelsModel(this))
{
    ui->setupUi(this);
	connect(ui->startButton, SIGNAL(clicked()), SLOT(onStartSearch()));
	connect(ui->filterEdit, SIGNAL(returnPressed()), SLOT(onStartSearch()));
	connect(ui->channelsView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onDoubleClick(QModelIndex)));
	ui->startButton->setIcon(Icon("media-playback-start"));
	ui->channelsView->setModel(m_model);
	ui->channelsView->setItemDelegate(new ChannelsDelegate(this));
}

IrcChannelListForm::~IrcChannelListForm()
{
    delete ui;
}

void IrcChannelListForm::listStarted()
{
	m_model->clear();
	ui->startButton->setEnabled(false);
	ui->filterEdit->setEnabled(false);
	m_count = 0;
	ui->infoLabel->setText(tr("Fetching channels list..."));
}

void IrcChannelListForm::listEnded()
{
	ui->startButton->setEnabled(true);
	ui->filterEdit->setEnabled(true);
	ui->infoLabel->setText(tr("Channels list loaded. (%1)").arg(m_count));
}

void IrcChannelListForm::addChannel(const QString &channel, const QString &users, const QString &topic)
{
	ui->infoLabel->setText(tr("Fetching channels list... (%1)").arg(++m_count));
	m_model->addChannel(channel, users, topic);
}

void IrcChannelListForm::error(const QString &error)
{
	ui->infoLabel->setText(error);
}

void IrcChannelListForm::onStartSearch()
{
	m_account->send(QString("LIST :%1").arg(ui->filterEdit->text()));
}

void IrcChannelListForm::onDoubleClick(const QModelIndex &index)
{
	if (!index.isValid())
		return;
	QString channelName = m_model->channels().value(index.row()).name;
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

