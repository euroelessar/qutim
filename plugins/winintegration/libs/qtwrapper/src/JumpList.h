/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef JUMPLIST_H
#define JUMPLIST_H

#include <QObject>
#include <QList>
#include <QFlags>
#include <QMultiMap>
#include <QIcon>

class JumpListItem;

class JumpList : public QObject
{
	Q_OBJECT

public:
	explicit JumpList(QObject *parent = 0);
	~JumpList();
	void clearList();
	//void addItem(const QString &category, const JumpListItem *item);
	void addTask(JumpListItem *item);
	JumpListItem* addItem(const QString &category, QIcon &icon, const QString &title, const QString &cmdLine, const QString &description = QString());
	JumpListItem* addTask(QIcon &icon, const QString &title, const QString &cmdLine, const QString &description = QString());
	void remove(JumpListItem *item);
	void removeCategory(const QString &category);
	void sync();

private:
	friend class JumpListItem;

	void onDescriptionChanged(JumpListItem*);
	void onTitleChanged(JumpListItem*);
	void onIconChanged(JumpListItem*);
	void onCmdPathChanged(JumpListItem*);
	void onCmdArgsChanged(JumpListItem*);

	QMultiMap<QString, QList<JumpListItem*> > m_items;
	QList<JumpListItem*> m_tasks;
};

class JumpListItem : public QObject
{
	Q_OBJECT

public:
	JumpListItem(){};
	~JumpListItem(){};
	void setTitle(const QString &title)
	{
		m_title = title;
		if (m_parentList)
			m_parentList->onTitleChanged(this);
	}
	void setDescription(const QString &description)
	{
		m_description = description;
		if (m_parentList)
			m_parentList->onDescriptionChanged(this);
	}
	void setIcon(const QIcon &icon)
	{
		m_icon = icon;
		if (m_parentList)
			m_parentList->onIconChanged(this);
	}
	void setObjectPath(const QString &path)
	{
		m_cmdPath = path;
		if (m_parentList)
			m_parentList->onCmdPathChanged(this);
	}
	void setCmdArguments(const QString &args)
	{
		m_cmdArgs = args;
		if (m_parentList)
			m_parentList->onCmdArgsChanged(this);
	}
	QString title() { return m_title; }
	QString description() { return m_description; }
	QIcon   icon() { return m_icon; }
	QString iconPath() { return m_nativeIconPath; }
	QString objectPath() { return m_cmdPath; }
	QString cmdArguments() { return m_cmdArgs; }
	void setCmdFromString(const QString &line) {	m_cmdPath = line.section(' ', 1, 1); m_cmdArgs = line.section(' ', 2); }

private:
	friend class JumpList;

	void setList(JumpList *l) { m_parentList = l; }
	bool generateNativeIcon();

	JumpList *m_parentList;
	QString m_title;
	QString m_description;
	QString m_cmdPath;
	QString m_cmdArgs;
	QIcon   m_icon;
	QString m_nativeIconPath;
};


#endif // JUMPLIST_H

