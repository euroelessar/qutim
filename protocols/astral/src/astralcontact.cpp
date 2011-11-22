/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "astralcontact.h"
#include "astralaccount.h"

struct AstralContactPrivate
{
	ContactPtr impl;
	Status status;
	QStringList tags;
};

AstralContact::AstralContact(AstralAccount *acc, const ContactPtr &impl) : qutim_sdk_0_3::Contact(acc), p(new AstralContactPrivate)
{
	p->impl = impl;
	connect(p->impl.data(), SIGNAL(aliasChanged(QString)), this, SIGNAL(nameChanged(QString)));
	connect(p->impl.data(), SIGNAL(simplePresenceChanged(QString,uint,QString)), this, SLOT(onStatusChanged()));
	connect(p->impl.data(), SIGNAL(addedToGroup(QString)), this, SLOT(onGroupsChanged()));
	connect(p->impl.data(), SIGNAL(removedFromGroup(QString)), this, SLOT(onGroupsChanged()));
	qDebug() << id() << name() << tags();
}

AstralContact::~AstralContact()
{
}

QString AstralContact::id() const
{
	return p->impl->id();
}

bool AstralContact::sendMessage(const qutim_sdk_0_3::Message &message)
{
//	p->impl->
	Q_UNUSED(message);
	return false;
}

QStringList AstralContact::tags() const
{
	return p->impl->groups();
}

QString AstralContact::name() const
{
	return p->impl->alias();
}

const ContactPtr &AstralContact::ptr()
{
	return p->impl;
}

void AstralContact::onStatusChanged()
{
	Status current = p->status;
	p->status = status();
	emit statusChanged(current, p->status);
}

void AstralContact::onGroupsChanged()
{
	QStringList current = p->tags;
	p->tags = tags();
	tagsChanged(current, p->tags);
}

Status AstralContact::status() const
{
//	switch(p->impl->presence().type())
//	{
//	case AstralUnknown:
//	case AstralError:
//	case AstralUnset:
//	case AstralOffline:
//		return Status::Offline;
//	case AstralAvailable:
//		return p->impl->presenceStatus() == QLatin1String("chat") ? Status::FreeChat : Status::Online;
//	case AstralAway:
//		return Status::Away;
//	case AstralExtendedAway:
//		return Status::NA;
//	case AstralHidden:
//		return Status::Invisible;
//	case AstralBusy:
//		return Status::DND;
//	}
//	return Status::Online;
}

void AstralContact::setName(const QString &name)
{
}

void AstralContact::setTags(const QStringList &tags)
{
	QSet<QString> oldTags = QSet<QString>::fromList(p->impl->groups());
	QSet<QString> newTags = QSet<QString>::fromList(tags);
	foreach (QString tag, oldTags - newTags)
		p->impl->removeFromGroup(tag)->deleteLater();
	foreach (QString tag, newTags - oldTags)
		p->impl->addToGroup(tag)->deleteLater();
}

bool AstralContact::isInList() const
{
	return true;
}

void AstralContact::setInList(bool inList)
{
}

