#include "autoreplydialog.h"
#include "ui_autoreplydialog.h"
#include "autoreplyplugin.h"
#include "autoreplymessagehandler.h"
#include <qutim/config.h>
#include <QPushButton>

using namespace qutim_sdk_0_3;

AutoReplyDialog::AutoReplyDialog(AutoReplyPlugin *plugin, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::AutoReplyDialog),
      m_plugin(plugin)
{
    ui->setupUi(this);

    ui->dateTimeEdit->setMinimumDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(60 * 15));
    Config config("autoreply");
	const QStringList messages = config.value("messages", QStringList());
    foreach (const QString &message, messages) {
        const QString firstLine = message.section('\n', 0, 0);
        ui->comboBox->addItem(firstLine, message);
    }
    ui->comboBox->setCurrentIndex(0);
    on_comboBox_currentIndexChanged(0);
}

AutoReplyDialog::~AutoReplyDialog()
{
    delete ui;
}

void AutoReplyDialog::accept()
{
    const int index = ui->comboBox->currentIndex();
    m_plugin->setReplyText(ui->comboBox->itemData(index).toString());
    m_plugin->setBackTime(ui->dateTimeEdit->dateTime());
    m_plugin->setActive(true);
    close();
}

void AutoReplyDialog::on_dateTimeEdit_dateTimeChanged()
{
    const int index = ui->comboBox->currentIndex();
    on_comboBox_currentIndexChanged(index);
}

void AutoReplyDialog::on_comboBox_currentIndexChanged(int index)
{
    QString message = ui->comboBox->itemData(index).toString();
    if (message.isEmpty()) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    AutoReplyMessageHandler::updateText(message, ui->dateTimeEdit->dateTime());
    ui->plainTextEdit->setPlainText(message);
}
