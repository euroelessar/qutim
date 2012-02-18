/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef JSERVERDISCOINFO_H
#define JSERVERDISCOINFO_H

//#include <gloox/connectionlistener.h>
//#include <gloox/discohandler.h>
//#include <QObject>

//namespace Jabber {

//	using namespace gloox;

//	class JAccount;

//	class JServerDiscoInfo: public QObject,
//							public ConnectionListener,
//							public DiscoHandler
//	{
//		Q_OBJECT
//	public:
//		JServerDiscoInfo(JAccount *account);
//		virtual ~JServerDiscoInfo();
//		// ConnectionListener
//		virtual void onConnect();
//		virtual void onDisconnect(ConnectionError error);
//		virtual bool onTLSConnect(const CertInfo &) { return true; }
//		// DiscoHandler
//		enum Contexts
//		{
//			ServerDiscoInfo
//		};
//		virtual void handleDiscoInfo(const JID &from, const Disco::Info &info, int context);
//		virtual void handleDiscoItems(const JID &from, const Disco::Items &items, int context);
//		virtual void handleDiscoError(const JID &from, const Error *error, int context);
//	private:
//		JAccount *m_account;
//	};

//} // namespace Jabber

#endif // JSERVERDISCOINFO_H

