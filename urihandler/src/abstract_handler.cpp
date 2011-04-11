#include <QMessageBox>
#include <QUrl>
#include <QTimer>

#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/chatsession.h>

#include "abstract_handler.h"
#include "account_selection.h"

using namespace qutim_sdk_0_3;

void AbstractHandler::open(QUrl &uri)
{
	OpenUriError status = OpenUriOk;
	Protocol *proto = protocol();
	if (!proto || !proto->accounts().size())
		status = NoAccounts;

	uri.setQueryDelimiters('=', ';');
	if (status == OpenUriOk)
		if (uri.isEmpty() || !uri.isValid())
			status = Invalid;
	if (status == OpenUriOk) {
		m_uri = uri;
		newUri(uri);
		status = testUri();
	}

	QString message;
	switch (status) {
	case ValidToOpen : {
		 Account *acc = ask(action(), uri.toString(QUrl::RemoveScheme | QUrl::RemoveQuery), description());
		if (acc)
			openImpl(acc);
		break;
	}
	case Invalid :
		message = QObject::tr("link is invalid or empty.");
		break;
	case NoAccounts :
		message = QObject::tr("no accounts to open link for.");
		break;
	case UnknownError :
	default:
		message = QObject::tr("unknown error.");
		break;
	}
	if (message.length())
		QMessageBox::warning(0, "qutIM", QObject::tr("Cannot open link: %1.").arg(message));
	// else all's ok
}

Account *AbstractHandler::ask(UriAction action, const QString &id, const ActionDescription &actionDescription)
{
	ActionDescription descriptionStrings = actionDescription;
	AccSelectionDlg dialog;
	QString description;
	QString question;
	switch (action) {
	case SendMessage :
		question = QObject::tr("Please, select account to send message to %1.").arg(id);
		break;
	case AddToRoster :
		question = QObject::tr("Please, select account to add %1 to roster.").arg(id);
		break;
	case RemoveFromRoster :
		question = QObject::tr("Please, select account to remove %1 from roster of that account.").arg(id);
		break;
	case Subscribe :
		question = QObject::tr("Please, select account to subscribe to %1.").arg(id);
		break;
	case Unsubscribe :
		question = QObject::tr("Please, select account to unsubscribe from %1.").arg(id);
		break;
	case NotSpecified :
		question = QObject::tr("Please, select account.");
		if (!descriptionStrings.length())
			descriptionStrings.append(ActionDescriptionItem(QObject::tr("Note"), QObject::tr("Action was not specified by link, so qutIM will simply open a chat with %1.").arg(id)));
		break;
	}
	foreach (const ActionDescriptionItem &item, descriptionStrings) {
		description += "<i>" + item.first + "</i>: " + item.second + "<br>";
	}
	dialog.setQuestion(question);
	dialog.setDescription(description);
	dialog.setAccountList(protocol()->accounts());

	int result = dialog.exec();
	if (result)
		return dialog.selectedAcc();
	else
		return 0;
}
