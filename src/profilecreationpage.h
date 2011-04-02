#ifndef PROFILECREATIONPAGE_H
#define PROFILECREATIONPAGE_H

#include <QWizardPage>
#include <QDir>

class QLineEdit;
namespace Ui {
	class ProfileCreationPage;
}

namespace Core
{
class ProfileCreationPage : public QWizardPage
{
	Q_OBJECT
public:
	ProfileCreationPage(QWidget *parent = 0);
	~ProfileCreationPage();
	virtual void initializePage();
	virtual bool validatePage();
	inline QByteArray cryptoName() const { return m_cryptoName; }

protected slots:
	void on_portableBox_toggled(bool portable);
	void on_idEdit_textChanged(const QString &text);
	void on_cryptoBox_currentIndexChanged(int index);
	void on_configBox_currentIndexChanged(int index);
	void onPathSelectTriggered();
protected:
	void rebaseDirs();
	bool createDirs(QString &);
	void changeEvent(QEvent *e);

private:
	QString m_password;
	QByteArray m_cryptoName;
	Ui::ProfileCreationPage *ui;
	bool m_singleProfile;
	bool m_is_valid;
};
}

#endif // PROFILECREATIONPAGE_H
