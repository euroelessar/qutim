#ifndef CHOOSECATEGORYPAGE_H
#define CHOOSECATEGORYPAGE_H

#include <QtGui/QWizardPage>

namespace Ui {
    class ChooseCategoryPage;
}

class GeneratorWindow;

class ChooseCategoryPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ChooseCategoryPage)
public:
	explicit ChooseCategoryPage(GeneratorWindow *parent = 0);
    virtual ~ChooseCategoryPage();
	virtual int nextId () const;
	virtual bool validatePage();

public slots:
	void on_comboBox_currentIndexChanged(int index);
	void on_browseButton_clicked();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ChooseCategoryPage *m_ui;
	GeneratorWindow *m_parent;
};

#endif // CHOOSECATEGORYPAGE_H
