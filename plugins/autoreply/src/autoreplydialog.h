#ifndef AUTOREPLYDIALOG_H
#define AUTOREPLYDIALOG_H

#include <QDialog>

class AutoReplyPlugin;

namespace Ui {
class AutoReplyDialog;
}

class AutoReplyDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AutoReplyDialog(AutoReplyPlugin *plugin, QWidget *parent = 0);
    ~AutoReplyDialog();

    virtual void accept();

private slots:
    void on_dateTimeEdit_dateTimeChanged();
    void on_comboBox_currentIndexChanged(int index);
    
private:
    Ui::AutoReplyDialog *ui;
    AutoReplyPlugin *m_plugin;
};

#endif // AUTOREPLYDIALOG_H
