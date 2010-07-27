#ifndef IRCCHANNELLIST_H
#define IRCCHANNELLIST_H

#include <QWidget>

namespace Ui {
	class IrcChannelListForm;
}

class QTreeWidgetItem;

namespace qutim_sdk_0_3 {

namespace irc {

class IrcAccount;

class IrcChannelListForm : public QWidget {
    Q_OBJECT
public:
	IrcChannelListForm(IrcAccount *account, QWidget *parent = 0);
	~IrcChannelListForm();
public slots:
	void listStarted();
	void listEnded();
	void addChannel(const QString &channel, const QString &users, const QString &topic);
	void error(const QString &error);
protected:
    void changeEvent(QEvent *e);
private slots:
	void onStartSearch();
	void onItemClicked(QTreeWidgetItem *item);
private:
	Ui::IrcChannelListForm *ui;
	IrcAccount *m_account;
	int m_count;
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCCHANNELLIST_H
