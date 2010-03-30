/****************************************************************************
 *  shortcut.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "shortcut.h"
#include "dglobalhotkey_p.h"
#include <QPointer>

namespace qutim_sdk_0_3
{
class GeneralShortcutInfo
{
public:
	LocalizedString name;
	LocalizedString group;
	QKeySequence key;
	bool global;
	bool inited;
protected:
	GeneralShortcutInfo(bool g) : global(g), inited(false) {}
};

class ShortcutInfo : public GeneralShortcutInfo
{
public:
	ShortcutInfo() : GeneralShortcutInfo(false), context(Qt::WindowShortcut) {}
	Qt::ShortcutContext context;
	QList<QPointer<Shortcut> > shortcuts;
};

class GlobalShortcutInfo : public GeneralShortcutInfo
{
public:
	GlobalShortcutInfo() : GeneralShortcutInfo(true) {}
	QList<int> contexts;
	QList<QPointer<GlobalShortcut> > shortcuts;
};

typedef QHash<QString, GeneralShortcutInfo *> ShortcutInfoHash;

struct ShortcutSelf
{
	ShortcutInfoHash hash;
	void init();
};

void ShortcutSelf::init()
{
	struct Info
	{
		const char *id;
		LocalizedString name;
		LocalizedString group;
		QKeySequence key;
		bool global;
		Qt::ShortcutContext context;
	} infos [] = {
	};
	Q_UNUSED(infos);
}

Q_GLOBAL_STATIC_WITH_INITIALIZER(ShortcutSelf, self, x->init())

class ShortcutPrivate
{
public:
	ShortcutInfo *info;
};

class GlobalShortcutPrivate
{
	Q_DECLARE_PUBLIC(GlobalShortcut)
public:
	GlobalShortcutPrivate(GlobalShortcut *q) : info(0), q_ptr(q) {}
	GlobalShortcutInfo *info;
	GlobalShortcut *q_ptr;
};

Shortcut::Shortcut(const QString &id, QWidget *parent) :
	QShortcut(parent), d_ptr(new ShortcutPrivate)
{
	Q_D(Shortcut);
	GeneralShortcutInfo *info = self()->hash.value(id);
	if (!info) {
		info = new ShortcutInfo();
		self()->hash.insert(id, info);
	}
	if (info && !info->global) {
		d->info = static_cast<ShortcutInfo*>(info);
		d->info->shortcuts.append(this);
		setKey(d->info->key);
		setContext(d->info->context);
	}
}

Shortcut::~Shortcut()
{
}

bool Shortcut::registerSequence(const QString &id, const LocalizedString &name,
							  const LocalizedString &group, const QKeySequence &key,
							  Qt::ShortcutContext context)
{
	ShortcutInfoHash &hash = self()->hash;
	ShortcutInfoHash::iterator it = hash.find(id);
	ShortcutInfo *info = 0;
	if (it != hash.end()) {
		GeneralShortcutInfo *i = it.value();
		if (i->global)
			return false;
		info = static_cast<ShortcutInfo*>(i);
	} else {
		info = new ShortcutInfo();
	}
	if (!info->inited) {
		info->inited = true;
		info->name = name;
		info->group = group;
		info->key = key;
		info->context = context;
		info->shortcuts.removeAll(NULL);
		foreach (Shortcut *shortcut, info->shortcuts) {
			shortcut->setKey(info->key);
			shortcut->setContext(info->context);
		}
		hash.insert(id, info);
		return true;
	}
	return false;
}

GlobalShortcut::GlobalShortcut(const QString &id, QObject *parent) :
	QObject(parent), d_ptr(new GlobalShortcutPrivate(this))
{
	Q_D(GlobalShortcut);
	GeneralShortcutInfo *info = self()->hash.value(id);
	if (!info) {
		info = new GlobalShortcutInfo();
		self()->hash.insert(id, info);
	}
	if (info->global) {
		d->info = static_cast<GlobalShortcutInfo*>(info);
		d->info->shortcuts.append(this);
		connect(dGlobalHotKey::instance(), SIGNAL(hotKeyPressed(quint32)),
				this, SLOT(onHotKeyPressed(quint32)));
	}
}

GlobalShortcut::~GlobalShortcut()
{
}

bool GlobalShortcut::registerSequence(const QString &id, const LocalizedString &name,
									const LocalizedString &group, const QKeySequence &key)
{
	ShortcutInfoHash &hash = self()->hash;
	ShortcutInfoHash::iterator it = hash.find(id);
	GlobalShortcutInfo *info = 0;
	if (it != hash.end()) {
		GeneralShortcutInfo *i = it.value();
		if (!i->global)
			return false;
		info = static_cast<GlobalShortcutInfo*>(i);
	} else {
		info = new GlobalShortcutInfo();
	}
	if (!info->inited) {
		info->inited = true;
		info->name = name;
		info->group = group;
		info->key = key;
		for (uint i = 0, count = key.count(); i < count; i++) {
			QString str = QKeySequence(key[i]).toString();
			int k = dGlobalHotKey::instance()->id(str);
			if (dGlobalHotKey::instance()->shortcut(str, true))
				info->contexts.append(k);
			else
				continue;
		}
		hash.insert(id, info);
		return true;
	}
	return false;
}

void GlobalShortcut::onHotKeyPressed(quint32 k)
{
	Q_D(GlobalShortcut);
	if (d->info && d->info->contexts.contains(k))
		emit activated();
}
}
