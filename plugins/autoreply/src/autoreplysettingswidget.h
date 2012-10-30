#ifndef AUTOREPLYSETTINGSWIDGET_H
#define AUTOREPLYSETTINGSWIDGET_H

#include <qutim/settingswidget.h>
#include <QListWidget>
#include <QPlainTextEdit>

namespace Ui {
class AutoReplySettingsWidget;
}

class AutoReplyTextLine : public QWidget
{
	Q_OBJECT
public:
	typedef QSharedPointer<QObject> Guard;
	AutoReplyTextLine(const QString &text, QListWidget *list);

	QString text() const;

protected slots:
	void onButtonClicked();

signals:
	void modified();

private:
	QPlainTextEdit *m_textEdit;
	QListWidgetItem *m_item;
};

class AutoReplySettingsWidget : public qutim_sdk_0_3::SettingsWidget
{
    Q_OBJECT
    
public:
	explicit AutoReplySettingsWidget();
    ~AutoReplySettingsWidget();

    void loadImpl();
    void saveImpl();
    void cancelImpl();

	void addReplyText(const QString &text = QString());

protected slots:
	void onModified();
	void on_addButton_clicked();
	void on_automaticBox_toggled(bool checked);

private:
    Ui::AutoReplySettingsWidget *ui;
};

#endif // AUTOREPLYSETTINGSWIDGET_H
