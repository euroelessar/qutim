#ifndef MIGRATIONSTARTPAGE_H
#define MIGRATIONSTARTPAGE_H

#include <QWizardPage>
#include <QDir>

namespace Ui {
    class MigrationStartPage;
}

namespace Core
{
class MigrationStartPage : public QWizardPage
{
	Q_OBJECT
public:
	struct ProtoInfo
	{
		ProtoInfo(QString l, QString n, QString h, QString c) :
				lower(l), name(n), history(h), config(c) {}
		QString lower;
		QString name;
		QString history;
		QString config;
	};

	MigrationStartPage(QWidget *parent = 0);
	~MigrationStartPage();

	virtual void initializePage();
	virtual bool validatePage();

protected slots:
	void on_profileBox_currentIndexChanged(int index);

protected:
	void changeEvent(QEvent *e);

private:
	QList<ProtoInfo> m_protos;
	Ui::MigrationStartPage *ui;
};
}

#endif // MIGRATIONSTARTPAGE_H
