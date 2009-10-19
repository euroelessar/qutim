#ifndef XSETTINGSDIALOG_H
#define XSETTINGSDIALOG_H

#include <QDialog>
#include "xsettingslayerimpl.h"

class XSettingsGroup;
class QStateMachine;
class QState;
class XToolBar;
namespace Ui {
    class XSettingsDialog;
}

class XSettingsDialog : public QDialog {
    Q_OBJECT
public:
    XSettingsDialog(const SettingsItemList &settings,QWidget *parent = 0);
    ~XSettingsDialog();
	//void setSettingsList(const SettingsItemList &settings);//TODO
protected:
    void changeEvent(QEvent *e);
	virtual void showEvent(QShowEvent* );
    virtual void hideEvent(QHideEvent* );
signals:
	void showed();
	void hided();
protected slots:
	void showState();
	void onActionTriggered(QAction *action);
private:
	inline void initAnimation();
	inline void addAction(QAction* action, Settings::Type type);
    Ui::XSettingsDialog *ui;
	QStateMachine *m_machine;
	QState *m_hide_state;
	QState *m_show_state;
	QVector<SettingsItemList> m_settings_items;
	QList<XSettingsGroup *> m_group_widgets;
	//configuration
	bool animated;
};

#endif // XSETTINGSDIALOG_H
