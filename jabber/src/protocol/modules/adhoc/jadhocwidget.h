#ifndef JADHOCWIDGET_H
#define JADHOCWIDGET_H

#include <QtGui/QRadioButton>
#include <QMap>
#include <gloox/jid.h>
#include <gloox/adhoc.h>
#include <gloox/adhochandler.h>
#include <QPointer>
#include "../../account/dataform/jdataform.h"
#include <qutim/localizedstring.h>
#include <qutim/icon.h>

//namespace Ui {
//class JAdHocWidget;
//}

//namespace Jabber
//{
//class JAdHocWidget : public QWidget, public gloox::AdhocHandler
//{
//	Q_OBJECT
//public:
//	JAdHocWidget(const gloox::JID &jid, gloox::Adhoc *adhoc, QWidget *parent = 0);
//	~JAdHocWidget();

//	void handleAdhocSupport(const gloox::JID &remote, bool support);
//	void handleAdhocCommands(const gloox::JID &remote, const gloox::StringMap &commands);
//	void handleAdhocError(const gloox::JID &remote, const gloox::Error *error);
//	void handleAdhocExecutionResult(const gloox::JID &remote, const gloox::Adhoc::Command &command);

//	void addButton(const QString &text, const QIcon &icon, const char *member);

//protected slots:
//	void doExecute();
//	void doCancel();
//	void doNext();
//	void doPrevious();
//	void doComplete();


//protected:
//	void changeEvent(QEvent *e);
//	void clear();

//private:
//	Ui::JAdHocWidget *m_ui;
//	gloox::JID m_jid;
//	gloox::Adhoc *m_adhoc;
//	std::string m_node;
//	std::string m_sessionId;
//	QMap<QRadioButton *, QString> m_options;
//	QPointer<JDataForm> m_dataForm;
//};
//}

#endif // JADHOCWIDGET_H
