#ifndef PROFILECREATIONPAGE_H
#define PROFILECREATIONPAGE_H

#include <QWizardPage>
#include <QDir>

namespace Ui {
	class ProfileCreationPage;
}

namespace Core
{
class ProfileCreationPage : public QWizardPage
{
	Q_OBJECT
public:
	ProfileCreationPage(const QString &password, QWidget *parent = 0);
	~ProfileCreationPage();

	virtual bool validatePage();

protected slots:
	void on_portableBox_toggled(bool portable);
	void on_idEdit_textChanged(const QString &text);
	void on_cryptoBox_currentIndexChanged(int index);
	void on_configBox_currentIndexChanged(int index);

protected:
	void rebaseDirs();
	void changeEvent(QEvent *e);

private:
	QString m_password;
	Ui::ProfileCreationPage *ui;
	bool m_is_valid;
};
}

#endif // PROFILECREATIONPAGE_H
