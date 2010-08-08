#ifndef ICQACCOUNTMAINSETTINGS_H
#define ICQACCOUNTMAINSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Ui {
    class IcqAccountMainSettings;
}

class QVBoxLayout;

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;

class IcqAccountMainSettings : public QWidget {
    Q_OBJECT
public:
	IcqAccountMainSettings(IcqAccount *account = 0, QWidget *parent = 0);
    ~IcqAccountMainSettings();
	void reloadSettings();
	void saveSettings();
	bool isComplete();
signals:
	void completeChanged();
protected:
    void changeEvent(QEvent *e);
	const QStringList &defaultServers() const;
	const QHash<QString, QString> &defaultSslServers() const;
private slots:
	void onSslChecked(bool checked);
	void onCurrentServerChanged(const QString &currentServer);
	void updatePort(bool ssl);
private:
	IcqAccount *m_account;
    Ui::IcqAccountMainSettings *ui;
};

class IcqAccountMainSettingsWindget: public SettingsWidget
{
	Q_OBJECT
public:
	IcqAccountMainSettingsWindget();
	virtual ~IcqAccountMainSettingsWindget();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private:
	IcqAccountMainSettings *m_widget;
	QVBoxLayout *m_layout;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQACCOUNTMAINSETTINGS_H
