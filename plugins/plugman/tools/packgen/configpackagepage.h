#ifndef CONFIGPACKAGEPAGE_H
#define CONFIGPACKAGEPAGE_H

#include <QtGui/QWizardPage>
#include <QHash>

namespace Ui {
    class ConfigPackagePage;
}

class GeneratorWindow;

class ConfigPackagePage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ConfigPackagePage)
public:
	explicit ConfigPackagePage(GeneratorWindow *parent = 0);
    virtual ~ConfigPackagePage();
	virtual int nextId() const;
	virtual void initializePage();
	virtual bool validatePage();

public slots:
	void on_nameLineEdit_textChanged(const QString &text);

protected:
	virtual void changeEvent(QEvent *e);

private:
    Ui::ConfigPackagePage *m_ui;
	GeneratorWindow *m_parent;
	QHash<QString, QString> m_packages;
};

#endif // CONFIGPACKAGEPAGE_H
