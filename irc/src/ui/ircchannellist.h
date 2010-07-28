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

#ifndef IRCCHANNELLIST_H
#define IRCCHANNELLIST_H

#include <QWidget>
#include <QAbstractListModel>
#include <QTextDocument>
#include <QStyledItemDelegate>

namespace Ui {
	class IrcChannelListForm;
}

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

struct Channel
{
	QString name;
	QString users;
	QTextDocument *topic;
};

class ChannelsModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ChannelsModel(QObject *parent = 0);
	~ChannelsModel();
	void addChannel(const QString &name, const QString &users, const QString &topic);
	void clear();
	QList<Channel> &channels() { return m_channels; }
protected:
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
protected:
	QList<Channel> m_channels;
};

class ChannelsDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit ChannelsDelegate(QWidget *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class IrcChannelListForm : public QWidget {
    Q_OBJECT
public:
	IrcChannelListForm(IrcAccount *account, QWidget *parent = 0);
	~IrcChannelListForm();
public slots:
	void listStarted();
	void listEnded();
	void addChannel(const QString &channel, const QString &users, const QString &topic);
	void error(const QString &error);
protected:
    void changeEvent(QEvent *e);
private slots:
	void onStartSearch();
	void onDoubleClick(const QModelIndex &index);
private:
	Ui::IrcChannelListForm *ui;
	IrcAccount *m_account;
	int m_count;
	ChannelsModel *m_model;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNELLIST_H
