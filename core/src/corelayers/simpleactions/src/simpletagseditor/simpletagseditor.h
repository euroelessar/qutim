#ifndef SIMPLETAGSEDITOR_H
#define SIMPLETAGSEDITOR_H

#include <QDialog>
#include <qutim/libqutim_global.h>
#include <QSet>

namespace Ui {
class SimpleTagsEditor;
}

namespace qutim_sdk_0_3
{
class Contact;
}

using namespace qutim_sdk_0_3;

namespace Core
{
class SimpleTagsEditor : public QDialog {
	Q_OBJECT
public:
	SimpleTagsEditor(Contact *contact);
	~SimpleTagsEditor();
	void accept();
public slots:
	void load();
private slots:
	void on_addButton_clicked();
	void save();
protected:
	void changeEvent(QEvent *e);
private:
	Ui::SimpleTagsEditor *ui;
	Contact *m_contact;
};

}
#endif // SIMPLETAGSEDITOR_H
