#include "jjidvalidator.h"
#include <gloox/jid.h>
#include <gloox/gloox.h>

namespace Jabber
{
	class JJidValidatorPrivate
	{
	public:
		std::string server;
	};

	JJidValidator::JJidValidator(const QString &server, QObject *parent)
			: QValidator(parent), d_ptr(new JJidValidatorPrivate)
	{
		d_func()->server = server.isNull() ? gloox::EmptyString : server.toStdString();
	}

	JJidValidator::JJidValidator(const std::string &server, QObject *parent)
			: QValidator(parent), d_ptr(new JJidValidatorPrivate)
	{
		d_func()->server = server;
	}

	JJidValidator::~JJidValidator()
	{
	}

	QString JJidValidator::server() const
	{
		return QString::fromStdString(d_func()->server);
	}

	QValidator::State JJidValidator::validate(QString &str, int &) const
	{
		Q_D(const JJidValidator);
		gloox::JID jid;
		if (d->server.empty()) {

			std::string jidFull = str.toStdString();
			jid.setJID(jidFull);
			if (str.indexOf('@') == str.size()-1)
				return Intermediate;
			if (!jid)
				return Invalid;
		} else {
			std::string jidNode = str.toStdString();
			if (str.indexOf('@') != -1) {
				jid.setJID(jidNode);
				jid.setServer(d->server);
			} else {
				jid.setUsername(jidNode);
				jid.setServer(d->server);
			}
			if (!jid)
				return Invalid;
		}
		fixup(str);
		return Acceptable;
	}

	void JJidValidator::fixup(QString &str) const
	{
		Q_D(const JJidValidator);
		gloox::JID jid;
		if (d->server.empty()) {
			jid.setJID(str.toStdString());
			str = QString::fromStdString(jid.bare());
		} else {
			std::string jidNode = str.toStdString();
			if (str.indexOf('@') != -1) {
				jid.setJID(jidNode);
				jid.setServer(d->server);
			} else {
				jid.setUsername(jidNode);
				jid.setServer(d->server);
			}
			str = QString::fromStdString(jid.username());
		}
	}
}
