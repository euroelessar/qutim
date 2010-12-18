#include "jjidvalidator.h"
#include <jreen/jid.h>

namespace Jabber
{
class JJidValidatorPrivate
{
public:
	QString server;
};

JJidValidator::JJidValidator(const QString &server, QObject *parent)
	: QValidator(parent), d_ptr(new JJidValidatorPrivate)
{
	d_func()->server = server;
}

JJidValidator::~JJidValidator()
{
}

QString JJidValidator::server() const
{
	return d_func()->server;
}

QValidator::State JJidValidator::validate(QString &str, int &) const
{
	Q_D(const JJidValidator);
	jreen::JID jid;
	if (d->server.isEmpty()) {
		jid.setJID(str);
		if (str.indexOf('@') == str.size()-1)
			return Intermediate;
		if (!jid.isValid())
			return Invalid;
	} else {
		if (str.indexOf('@') != -1) {
			if (jid.setJID(str))
				jid.setDomain(d->server);
		} else {
			if (jid.setNode(str))
				jid.setDomain(d->server);
		}
		if (!jid.isValid())
			return Invalid;
	}
	fixup(str);
	return Acceptable;
}

void JJidValidator::fixup(QString &str) const
{
	Q_D(const JJidValidator);
	jreen::JID jid;
	if (d->server.isEmpty()) {
		if (jid.setJID(str))
			str = jid.bare();
	} else {
		if (str.indexOf('@') != -1) {
			if (jid.setJID(str))
				jid.setDomain(d->server);
		} else {
			if (jid.setNode(str))
				jid.setDomain(d->server);
		}
		str = jid.isValid() ? jid.node() : str;
	}
}
}
