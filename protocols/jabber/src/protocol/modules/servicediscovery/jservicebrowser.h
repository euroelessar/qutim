#ifndef JSERVICEBROWSER_H
#define JSERVICEBROWSER_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QKeyEvent>
#include <qutim/icon.h>
#include "../../../sdk/jabber.h"
#include <jreen/disco.h>

namespace Jabber
{
class JAccount;
class JDiscoItem;
class JServiceDiscovery;

struct JServiceBrowserPrivate;

class JServiceBrowserModule : public QObject, public JabberExtension
{
	Q_OBJECT
	Q_INTERFACES(Jabber::JabberExtension)
public:
	JServiceBrowserModule();
	virtual void init(qutim_sdk_0_3::Account *account);
public slots:
	void showWindow();
private:
	qutim_sdk_0_3::Account *m_account;
//	QWeakPointer<QWidget> m_widget;
};

class JServiceBrowser : public QWidget
{
	Q_OBJECT
public:
	JServiceBrowser(qutim_sdk_0_3::Account *account, bool isConference = false, QWidget *parent = 0);
	~JServiceBrowser();
private slots:
	void getItems(QTreeWidgetItem *item);
	void showContextMenu(const QPoint &pos);
	void showFeatures();
	void filterItem(const QString &mask);
	void closeEvent(QCloseEvent*);
	void on_searchButton_clicked();
	void on_clearButton_clicked();
	void onExecute();
	void onJoin();
	void onAddToRoster();
	void onInfoReceived(const Jreen::Disco::Item &item);
	void onItemsReceived(const Jreen::Disco::ItemList &items);
	void onError(const Jreen::Error::Ptr &error);
	/*void on_registerButton_clicked();
   void on_searchFormButton_clicked();
   void on_executeButton_clicked();
   void on_showVCardButton_clicked();
   void on_addProxyButton_clicked();*/
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void searchServer(const QString &server);
	void getInfo(QTreeWidgetItem *item);
	void setItemVisible(QTreeWidgetItem *item, bool visibility);
	QList<QTreeWidgetItem *> findItems(QTreeWidgetItem *item, const QString &text);
	void setBranchVisible(QList<QTreeWidgetItem *> items);
	QString serviceIcon(const Jreen::Disco::Item &di);
private:
	QScopedPointer<JServiceBrowserPrivate> p;
signals:
	void joinConference(const QString &conference); //WTF ?
	/*void registerTransport(const QString &transport);
   void executeCommand(const QString &command, const QString &node);
   void showVCard(const QString &jid);
   void addProxy(const JID &proxy);
   void addContact(const QString &jid, const QString &nick);
   void searchService(const QString &type, const QString &jid);*/
};
}

Q_DECLARE_METATYPE(QTreeWidgetItem*)

#endif // JSERVICEBROWSER_H
