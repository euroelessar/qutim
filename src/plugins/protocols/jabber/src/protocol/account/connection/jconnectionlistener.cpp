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
//#include "jconnectionlistener.h"
//#include "../jaccount.h"
//#include "../../jprotocol.h"
//#include "../muc/jmucsession.h"
//#include "../vcard/jvcardmanager.h"
//#include <qutim/notification.h>
//#include <qutim/chatsession.h>
//#include <QDebug>

//namespace Jabber
//{
//	using namespace gloox;
//struct JConnectionListenerPrivate
//{
//	JAccount *account;
//};

//JConnectionListener::JConnectionListener(JAccount *account) :
//	QObject(account),
//	ConnectionListener(),
//	p(new JConnectionListenerPrivate)
//{
//	p->account = account;
//	//p->account->connection()->client()->registerConnectionListener(this);
//}

//JConnectionListener::~JConnectionListener()
//{
//}

//void JConnectionListener::onConnect()
//{
//	//p->account->connection()->vCardManager()->fetchVCard(p->account->id());
//	//p->account->endChangeStatus(p->account->client()->presence().presence()); //doesn't work correctly((
//}

//void JConnectionListener::onDisconnect(ConnectionError error)
//{
//	//		p->account->client()->setCompression();
//	//p->account->endChangeStatus(Presence::Unavailable);
//	//if (error != ConnNoError && error != ConnUserDisconnected && error != ConnNotConnected) {
//	//	QString s;
//	//	switch (error) {
//	//	case ConnStreamError:
//	//		switch (p->account->connection()->client()->streamError()) {
//	//		case StreamErrorBadFormat:
//	//			s = tr("The entity has sent XML that cannot be processed.");
//	//			break;
//	//		case StreamErrorBadNamespacePrefix:
//	//			s = tr("The entity has sent a namespace prefix that is unsupported, or has sent no namespace prefix on an element that requires such a prefix.");
//	//			break;
//	//		case StreamErrorConflict:
//	//			s = tr("The server is closing the active stream for this entity because a new stream has been initiated that conflicts with the existing stream.");
//	//			break;
//	//		case StreamErrorConnectionTimeout:
//	//			s = tr("The entity has not generated any traffic over the stream for some period of time (configurable according to a local service policy).");
//	//			break;
//	//		case StreamErrorHostGone:
//	//			s = tr("The value of the 'to' attribute provided by the initiating entity in the stream header corresponds to a hostname that is no longer hosted by the server.");
//	//			break;
//	//		case StreamErrorHostUnknown:
//	//			s = tr("The value of the 'to' attribute provided by the initiating entity in the stream header does not correspond to a hostname that is hosted by the server.");
//	//			break;
//	//		case StreamErrorImproperAddressing:
//	//			s = tr("A stanza sent between two servers lacks a 'to' or 'from' attribute (or the attribute has no value).");
//	//			break;
//	//		case StreamErrorInternalServerError:
//	//			s = tr("The server has experienced a misconfiguration or an otherwise-undefined internal error that prevents it from servicing the stream.");
//	//			break;
//	//		case StreamErrorInvalidFrom:
//	//			s = tr("The JID or hostname provided in a 'from' address does not match an authorized JID or validated domain negotiated between servers via SASL or dialback, or between a client and a server via authentication and resource binding.");
//	//			break;
//	//		case StreamErrorInvalidId:
//	//			s = tr("The stream ID or dialback ID is invalid or does not match an ID previously provided.");
//	//			break;
//	//		case StreamErrorInvalidNamespace:
//	//			s = tr("The streams namespace name is something other than 'http://etherx.jabber.org/streams' or the dialback namespace name is something other than 'jabber:server:dialback'.");
//	//			break;
//	//		case StreamErrorInvalidXml:
//	//			s = tr("The entity has sent invalid XML over the stream to a server that performs validation.");
//	//			break;
//	//		case StreamErrorNotAuthorized:
//	//			s = tr("The entity has attempted to send data before the stream has been authenticated, or otherwise is not authorized to perform an action related to stream negotiation.");
//	//			break;
//	//		case StreamErrorPolicyViolation:
//	//			s = tr("The entity has violated some local service policy.");
//	//			break;
//	//		case StreamErrorRemoteConnectionFailed:
//	//			s = tr("The server is unable to properly connect to a remote entity that is required for authentication or authorization.");
//	//			break;
//	//		case StreamErrorResourceConstraint:
//	//			s = tr("The server lacks the system resources necessary to service the stream.");
//	//			break;
//	//		case StreamErrorRestrictedXml:
//	//			s = tr("The entity has attempted to send restricted XML features such as a comment, processing instruction, DTD, entity reference, or unescaped character.");
//	//			break;
//	//		case StreamErrorSeeOtherHost:
//	//			s = tr("The server will not provide service to the initiating entity but is redirecting traffic to another host.");
//	//			break;
//	//		case StreamErrorSystemShutdown:
//	//			s = tr("The server is being shut down and all active streams are being closed.");
//	//			break;
//	//		case StreamErrorUndefinedCondition:
//	//			s = tr("The error condition is not one of those defined by the other conditions in this list.");
//	//			break;
//	//		case StreamErrorUnsupportedEncoding:
//	//			s = tr("The initiating entity has encoded the stream in an encoding that is not supported by the server.");
//	//			break;
//	//		case StreamErrorUnsupportedStanzaType:
//	//			s = tr("The initiating entity has sent a first-level child of the stream that is not supported by the server.");
//	//			break;
//	//		case StreamErrorUnsupportedVersion:
//	//			s = tr("The value of the 'version' attribute provided by the initiating entity in the stream header specifies a version of XMPP that is not supported by the server.");
//	//			break;
//	//		case StreamErrorXmlNotWellFormed:
//	//			s = tr("The initiating entity has sent XML that is not well-formed as defined by [XML].");
//	//			break;
//	//		case StreamErrorUndefined:
//	//			// FIX IT
//	//			// need or not?
//	//			s = tr("An undefined/unknown error occured.");
//	//			break;
//	//		default:
//	//			s = tr("WHAT?! Unknown error?! OH SHI~~");
//	//			break;
//	//		}
//	//		break;
//	//	case ConnStreamVersionError:
//	//		s = tr("The incoming stream's version is not supported.");
//	//		break;
//	//	case ConnStreamClosed:
//	//		s = tr("The stream has been closed (by the server).");
//	//		break;
//	//	case ConnProxyAuthRequired:
//	//		s = tr("The HTTP/SOCKS5 proxy requires authentication.");
//	//		break;
//	//	case ConnProxyAuthFailed:
//	//		s = tr("HTTP/SOCKS5 proxy authentication failed.");
//	//		break;
//	//	case ConnProxyNoSupportedAuth:
//	//		s = tr("The HTTP/SOCKS5 proxy requires an unsupported auth mechanism.");
//	//		break;
//	//	case ConnIoError:
//	//		s = tr("An I/O error occured.");
//	//		break;
//	//	case ConnParseError:
//	//		s = tr("An XML parse error occurred.");
//	//		break;
//	//	case ConnConnectionRefused:
//	//		s = tr("The connection was refused by the server (on the socket level).");
//	//		break;
//	//	case ConnSocketError:
//	//		s = tr("Creation of the socket failed.");
//	//		break;
//	//	case ConnDnsError:
//	//		s = tr("Resolving the server's hostname failed.");
//	//		break;
//	//	case ConnOutOfMemory:
//	//		s = tr("Out of memory. Uhoh.");
//	//		break;
//	//	case ConnNoSupportedAuth:
//	//		s = tr("The auth mechanisms the server offers are not supported or the server offered no auth mechanisms at all.");
//	//		break;
//	//	case ConnTlsFailed:
//	//		s = tr("The server's certificate could not be verified or the TLS handshake did not complete successfully.");
//	//		break;
//	//	case ConnTlsNotAvailable:
//	//		s = tr("The server didn't offer TLS while it was set to be required, or TLS was not compiled in.");
//	//		break;
//	//	case ConnCompressionFailed:
//	//		s = tr("Negotiating/initializing compression failed.");
//	//		break;
//	//	case ConnCompressionNotAvailable:
//	//		s = tr("TLS was required but is not compiled in.");
//	//		break;
//	//	case ConnAuthenticationFailed:
//	//		switch (p->account->connection()->client()->authError()) {
//	//		case AuthErrorUndefined:
//	//			// FIX IT
//	//			// need or not?
//	//			s = tr("Authentication failed.");
//	//			break;
//	//		case SaslAborted:
//	//			s = tr("The receiving entity acknowledges an <abort/> element sent by the initiating entity.");
//	//			break;
//	//		case SaslIncorrectEncoding:
//	//			s = tr("The data provided by the initiating entity could not be processed.");
//	//			break;
//	//		case SaslInvalidAuthzid:
//	//			s = tr("The authzid provided by the initiating entity is invalid.");
//	//			break;
//	//		case SaslInvalidMechanism:
//	//			s = tr("The initiating entity did not provide a mechanism or requested a mechanism that is not supported by the receiving entity.");
//	//			break;
//	//		case SaslMalformedRequest:
//	//			s = tr("The request is malformed.");
//	//			break;
//	//		case SaslMechanismTooWeak:
//	//			s = tr("The mechanism requested by the initiating entity is weaker than server policy permits for that initiating entity.");
//	//			break;
//	//		case SaslNotAuthorized:
//	//			s = tr("The authentication failed because the initiating entity did not provide valid credentials (this includes but is not limited to the case of an unknown username).");
//	//			break;
//	//		case SaslTemporaryAuthFailure:
//	//			s = tr("The authentication failed because of a temporary error condition within the receiving entity.");
//	//			break;
//	//		case NonSaslConflict:
//	//			s = tr("XEP-0078: Resource Conflict.");
//	//			break;
//	//		case NonSaslNotAcceptable:
//	//			s = tr("XEP-0078: Required Information Not Provided.");
//	//			break;
//	//		case NonSaslNotAuthorized:
//	//			s = tr("XEP-0078: Incorrect Credentials.");
//	//			break;
//	//		default:
//	//			s = tr("WHAT?! Unknown error?! OH SHI~~");
//	//			break;
//	//		}
//	//		break;
//	//	default:
//	//		s = tr("WHAT?! Unknown error?! OH SHI~~");
//	//		break;
//	//	}
//	//	// somes actions with s
//	//	Notifications::send(s, tr("System error"));
//	//	qDebug() << "ERROR" << s;
//	//}
//}

//void JConnectionListener::onResourceBind(const std::string &resource)
//{
//}

//void JConnectionListener::onResourceBindError(const Error *error)
//{
//	if (!error)
//		return;
//}

//void JConnectionListener::onSessionCreateError(const Error *error)
//{
//	if (!error)
//		return;
//}

//bool JConnectionListener::onTLSConnect(const CertInfo &info)
//{
//	//Config c = Config(p->account->protocol()->id() + QLatin1Char('.') + p->account->id() + QLatin1String("/certificates"));
//	//QString s = QString::fromStdString(info.server);

//	//QVariant value = c.value(s);
//	//if (value.canConvert(QVariant::Bool))
//	//	return value.toBool();

//	//if (!p->account->config().group("connect").value("viewCertInfo", false))
//	//	return true;

//	//bool r;
//	//if (JCertInfo(info).exec(r)) {
//	//	c.setValue(s, r);
//	//	c.sync();
//	//}

//	//if (r)
//	//	return true;
//	//else
//	//	return false;
//	return false;
//}

//void JConnectionListener::onStreamEvent(StreamEvent event)
//{
//}
//}

