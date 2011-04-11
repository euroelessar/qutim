#ifndef ABSTRACT_HADLER_H
#define ABSTRACT_HADLER_H

#include <QPair>
#include <QList>
#include <QUrl>
#include <QMap>

namespace qutim_sdk_0_3
{
	class Protocol;
	class Account;
}

class AbstractHandler
{
public:
	void open(QUrl &url);
	virtual ~AbstractHandler() {}

protected:
	enum OpenUriError
	{
		OpenUriOk,
		ValidToOpen = OpenUriOk,
		Invalid,
		Empty = Invalid,
		NoAccounts, // no accounts to open sessions for
		UnknownError
	};

	enum UriAction
	{
		SendMessage,
		AddToRoster,
		RemoveFromRoster,
		Subscribe,
		Unsubscribe,
		NotSpecified/*,
		UnknownAction*/
	};

	typedef QPair<QString, QString> ActionDescriptionItem;
	typedef QList<ActionDescriptionItem> ActionDescription;

	virtual qutim_sdk_0_3::Protocol* protocol() = 0;
	virtual void openImpl(qutim_sdk_0_3::Account *acc) = 0;
	virtual ActionDescription description() { return ActionDescription(); }
	virtual void newUri(QUrl &uri) = 0;
	virtual UriAction action() = 0;
	virtual OpenUriError testUri(){ return ValidToOpen; }

	qutim_sdk_0_3::Account *ask(UriAction action, const QString &id,
								const ActionDescription &description);
	QUrl &uri() { return m_uri; }

private:
	QUrl m_uri;
};

#endif // ABSTRACT_HADLER_H
