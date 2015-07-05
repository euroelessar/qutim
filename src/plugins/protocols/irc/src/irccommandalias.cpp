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

#include "irccommandalias.h"
#include "ircactiongenerator.h"
#include "ircprotocol.h"
#include "ircchannelparticipant.h"
#include <qutim/menucontroller.h>
#include <QRegExp>
#include <QStringList>
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcPingAlias : public IrcCommandAlias
{
public:
	IrcPingAlias();
	virtual QString generate(IrcCommandAlias::Type aliasType, const QStringList &params,
							 const QHash<QChar, QString> &extParams, QString *error = 0) const;
};

class IrcCommandAliasPrivate : public QSharedData
{
public:
	IrcCommandAliasPrivate(const QString &_name, const QString &_command, IrcCommandAlias::Types _types);
	QString name;
	QString command;
	IrcCommandAlias::Types types;
};

IrcCommandAliasPrivate::IrcCommandAliasPrivate(const QString &_name,
						const QString &_command, IrcCommandAlias::Types _types) :
	name(_name),
	command(_command),
	types(_types)
{
}

IrcCommandAlias::IrcCommandAlias(const QString &name, const QString &command, Types types) :
	d(new IrcCommandAliasPrivate(name, command, types))
{
}

IrcCommandAlias::~IrcCommandAlias()
{
}

QString IrcCommandAlias::name() const
{
	return d->name;
}

QString IrcCommandAlias::generate(IrcCommandAlias::Type aliasType, const QStringList &params,
								  const QHash<QChar, QString> &extParams, QString *error) const
{
	if (!(d->types & aliasType))
		return QString();
	static QRegExp paramRx("%([0-9]{1,2}|[a-zA-Z])(-|)");
	QString command = d->command; // the new command
	int pos = 0;
	while ((pos = paramRx.indexIn(command, pos)) != -1) {
		bool isIndex;
		int index = paramRx.cap(1).toInt(&isIndex);
		QString param; // next parameter in the new command
		if (isIndex) {
			// Replace template by parameter(s) from the command line
			Q_ASSERT(index >= 0);
			if (index >= params.size())
				goto Lerr;
			if (paramRx.cap(2) != "-") {
				param = params.at(index);
			} else {
				for (int i = index, c = params.size(); i < c; ++i) {
					if (i != index)
						param += " ";
					param += params.at(i);
				}
			}
		} else {
			// Replace template by built-in parameter
			QChar extParamIndex = paramRx.cap(1).at(0);
			if (extParamIndex == '%') {
				pos += 1;
				continue;
			} else if (extParams.contains(extParamIndex)) {
				param = extParams.value(extParamIndex);
			} else {
				goto Lerr;
			}
		}
		command.replace(pos, paramRx.matchedLength(), param);
		pos += param.size();
	}
	return command;
Lerr:
	if (error) *error = QObject::tr("Not enough parameters for command %1").arg(d->name);
	return QString();
}


///////////////////////////////////////////////////////////////////////////////////
// Initialization of the standart aliases
///////////////////////////////////////////////////////////////////////////////////

IrcPingAlias::IrcPingAlias() :
	IrcCommandAlias("ping", QString())
{
}

QString IrcPingAlias::generate(IrcCommandAlias::Type aliasType, const QStringList &params,
							   const QHash<QChar, QString> &extParams, QString *error) const
{
	Q_UNUSED(aliasType);
	Q_UNUSED(error);
	QString user = extParams.value('o');
	if (user.isEmpty())
		user = params.value(0);
	if (user.isEmpty())
		return QString();
	QDateTime current = QDateTime::currentDateTime();
	QString timeStamp = QString("%1.%2").arg(current.toTime_t()).arg(current.time().msec());
	return QStringLiteral("PRIVMSG %1 :\001PING %2\001").arg(user).arg(timeStamp);
}

#define ADD_BAN_CMD(MODE, TYPE, NAME, TITLE, ADDITIONALCMD) \
	cmd = new IrcCommandAlias(NAME, "MODE %n +b " MODE ADDITIONALCMD, IrcCommandAlias::Participant); \
	gen = new IrcActionGenerator(QIcon(), TITLE, cmd); \
	gen->setType(TYPE); \
	MenuController::addAction<IrcChannelParticipant>(gen, kickBanGroup);

#define ADD_BAN_CMD_EXT(MODE, TYPE, NAME, TITLE, ADDITIONALCMD) \
	ADD_BAN_CMD(MODE, TYPE, NAME, TITLE, ADDITIONALCMD) \
	gen->enableAutoDeleteOfCommand();


#define REGISTER_CTCP_CMD(TITLE) \
	gen = new IrcActionGenerator(QIcon(), TITLE, cmd); \
	gen->setType(0x00002); \
	gen->setPriority(80); \
	MenuController::addAction<IrcChannelParticipant>(gen); \
	registerAlias(cmd);

#define CREATE_CTCP_CMD(NAME, CMD, TITLE) \
	cmd = new IrcCommandAlias(NAME, "PRIVMSG %o :\001" CMD "\001", \
							  IrcCommandAlias::Participant | \
							  IrcCommandAlias::PrivateChat); \
	REGISTER_CTCP_CMD(TITLE)

#define ADD_MODE(MODE, PRIORITY, NAME, TITLE)\
	cmd = new IrcCommandAlias(NAME, "MODE %n " MODE " %o", IrcCommandAlias::Participant);\
	gen = new IrcActionGenerator(QIcon(), TITLE, cmd);\
	gen->setPriority(PRIORITY);\
	MenuController::addAction<IrcChannelParticipant>(gen, modesGroup);\
	registerAlias(cmd);

inline void registerAlias(IrcCommandAlias *alias)
{
	IrcProtocol::registerCommandAlias(alias);
}

void IrcCommandAlias::initStandartAliases()
{
	IrcCommandAlias *cmd = 0;
	IrcActionGenerator *gen = 0;

	registerAlias(new IrcCommandAlias("ctcp", "PRIVMSG %1 :\001%2-\001"));
	registerAlias(new IrcCommandAlias("me", "PRIVMSG %1 :\001ACTION %2-\001",
								  IrcCommandAlias::Console));
	registerAlias(new IrcCommandAlias("me", "PRIVMSG %n :\001ACTION %0\001",
								  IrcCommandAlias::Channel | IrcCommandAlias::PrivateChat));
	registerAlias(new IrcCommandAlias("cs", "PRIVMSG ChanServ :%0"));
	registerAlias(new IrcCommandAlias("ns", "PRIVMSG NickServ :%0"));
	registerAlias(new IrcCommandAlias("kick", "KICK %n %0",
								  IrcCommandAlias::Channel));
	registerAlias(new IrcCommandAlias("ban", "MODE %n +b %0",
								  IrcCommandAlias::Channel));
	registerAlias(new IrcCommandAlias("msg", "PRIVMSG %0"));
	registerAlias(new IrcCommandAlias("clientinfo", "PRIVMSG %1 :\001CLIENTINFO\001"));
	registerAlias(new IrcCommandAlias("version", "PRIVMSG %1 :\001VERSION\001"));
	registerAlias(new IrcCommandAlias("time", "PRIVMSG %1 :\001TIME\001"));
	registerAlias(new IrcCommandAlias("avatar", "PRIVMSG %1 :\001AVATAR\001"));

	// Kick and ban commands

	const QList<QByteArray> kickBanGroup = QList<QByteArray>() << QT_TR_NOOP("Kick / Ban").original();

	cmd = new IrcCommandAlias("kick", "KICK %n %o", IrcCommandAlias::Participant);
	gen = new IrcActionGenerator(QIcon(), QT_TR_NOOP("Kick"), cmd);
	gen->setType(0x00001);
	MenuController::addAction<IrcChannelParticipant>(gen, kickBanGroup);
	registerAlias(cmd);

	ADD_BAN_CMD("%o", 0x00001, "kickban", QT_TR_NOOP("Kickban"), "\nKICK %n %o")
	registerAlias(cmd);

	ADD_BAN_CMD("%o", 0x00001, "ban", QT_TR_NOOP("Ban"), "")
	registerAlias(cmd);

	// Extended ban commands
	ADD_BAN_CMD_EXT("*!*@*.%h", 0x00002, "ban", QT_TR_NOOP("Ban *!*@*.host"), "")
	ADD_BAN_CMD_EXT("*!*@%d", 0x00002, "ban", QT_TR_NOOP("Ban *!*@domain"), "")
	ADD_BAN_CMD_EXT("*!%u@*.%h", 0x00002, "ban", QT_TR_NOOP("Ban *!user@*.host"), "")
	ADD_BAN_CMD_EXT("*!%u@%d", 0x00002, "ban", QT_TR_NOOP("Ban *!user@domain"), "")

	// Exntended kickban commands
	ADD_BAN_CMD_EXT("*!*@*.%h", 0x00003, "kickban", QT_TR_NOOP("Kickban *!*@*.host"), "\nKICK %n %o")
	ADD_BAN_CMD_EXT("*!*@%d", 0x00003, "kickban", QT_TR_NOOP("Kickban *!*@domain"), "\nKICK %n %o")
	ADD_BAN_CMD_EXT("*!%u@*.%h", 0x00003, "kickban", QT_TR_NOOP("Kickban *!user@*.host"), "\nKICK %n %o")
	ADD_BAN_CMD_EXT("*!%u@%d", 0x00003, "kickban", QT_TR_NOOP("Kickban *!user@domain"), "\nKICK %n %o")

	// CTCP commands
	cmd = new IrcPingAlias;
	REGISTER_CTCP_CMD(QT_TR_NOOP("Ping"));
	CREATE_CTCP_CMD("clientinfo", "CLIENTINFO", QT_TR_NOOP("Request client information"));
	CREATE_CTCP_CMD("version", "VERSION", QT_TR_NOOP("Request version"));
	CREATE_CTCP_CMD("time", "TIME", QT_TR_NOOP("Request time"));
	CREATE_CTCP_CMD("avatar", "AVATAR", QT_TR_NOOP("Avatar"));

	// Modes commands
	const QList<QByteArray> modesGroup = QList<QByteArray>() << QT_TR_NOOP("Modes").original();
	ADD_MODE("+o", 60, "op", QT_TR_NOOP("Give Op"));
	ADD_MODE("-o", 59, "deop", QT_TR_NOOP("Take Op"));
	ADD_MODE("+h", 58, "hop", QT_TR_NOOP("Give HalfOp"));
	ADD_MODE("-h", 57, "dehop", QT_TR_NOOP("Take HalfOp"));
	ADD_MODE("+v", 56, "voice", QT_TR_NOOP("Give voice"));
	ADD_MODE("-v", 55, "devoice", QT_TR_NOOP("Take voice"));

	// Other commands
	cmd = new IrcCommandAlias("whois", "WHOIS %o", Participant | PrivateChat);
	gen = new IrcActionGenerator(QIcon(), QT_TR_NOOP("Send WHOIS request"), cmd);
	MenuController::addAction<IrcChannelParticipant>(gen);
	gen->setType(0x00001);
	registerAlias(cmd);
}

}

} // namespace qutim_sdk_0_3::irc

