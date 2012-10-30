#include "autoreplysettingswidget.h"
#include "ui_autoreplysettingswidget.h"
#include <qutim/config.h>
#include <qutim/icon.h>
#include <QHBoxLayout>
#include <QTime>
#include <QPushButton>
#include <QDebug>

Q_DECLARE_METATYPE(AutoReplyTextLine::Guard)

using namespace qutim_sdk_0_3;

AutoReplyTextLine::AutoReplyTextLine(const QString &text, QListWidget *list)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	m_textEdit = new QPlainTextEdit(this);

	QPushButton *button = new QPushButton(tr("Remove"), this);
	button->setIcon(qutim_sdk_0_3::Icon(QLatin1String("list-remove")));
	layout->addWidget(m_textEdit);
	layout->addWidget(button);
	connect(button, SIGNAL(clicked()), SLOT(onButtonClicked()));
	connect(m_textEdit, SIGNAL(textChanged()), SIGNAL(modified()));

	QSizePolicy policy = m_textEdit->sizePolicy();
	policy.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	m_textEdit->setSizePolicy(policy);
	m_textEdit->setMaximumHeight(100);

	m_item = new QListWidgetItem(list);
	QObject *guard = new QObject();
	connect(guard, SIGNAL(destroyed()), SLOT(deleteLater()));
	m_item->setData(Qt::UserRole, qVariantFromValue(Guard(guard)));
	m_item->setData(Qt::SizeHintRole, sizeHint());
	m_textEdit->setPlainText(text);
	list->setItemWidget(m_item, this);
}

QString AutoReplyTextLine::text() const
{
	return m_textEdit->toPlainText();
}

void AutoReplyTextLine::onButtonClicked()
{
	emit modified();
	delete m_item;
}

AutoReplySettingsWidget::AutoReplySettingsWidget()
	: ui(new Ui::AutoReplySettingsWidget)
{
	ui->setupUi(this);
	lookForWidgetState(ui->automaticBox);
	lookForWidgetState(ui->timeEdit);
	lookForWidgetState(ui->deltaTimeEdit);
	lookForWidgetState(ui->plainTextEdit);
}

AutoReplySettingsWidget::~AutoReplySettingsWidget()
{
    delete ui;
}


void AutoReplySettingsWidget::loadImpl()
{
	Config config("autoreply");
	const QTime time(0, 0, 0);
	ui->timeEdit->setTime(time.addSecs(config.value("timeOut", 60 * 15)));
	ui->deltaTimeEdit->setTime(time.addSecs(config.value("deltaTime", 60 * 15)));
	ui->automaticBox->setChecked(config.value("automatic", true));
	ui->plainTextEdit->setPlainText(config.value("message", QString()));
	const QStringList messages = config.value("messages", QStringList());
	foreach (const QString &text, messages)
		addReplyText(text);
}

void AutoReplySettingsWidget::saveImpl()
{
	Config config("autoreply");
	const QTime time(0, 0, 0);
	config.setValue("timeOut", time.secsTo(ui->timeEdit->time()));
	config.setValue("deltaTime", time.secsTo(ui->deltaTimeEdit->time()));
	config.setValue("automatic", ui->automaticBox->isChecked());
	config.setValue("message", ui->plainTextEdit->toPlainText());
	QStringList messages;
	for (int i = 0; i < ui->answersList->count(); ++i) {
		QListWidgetItem *item = ui->answersList->item(i);
		QWidget *widget = ui->answersList->itemWidget(item);
		AutoReplyTextLine *line = qobject_cast<AutoReplyTextLine*>(widget);
		messages << line->text();
	}
	config.setValue("messages", messages);
}

void AutoReplySettingsWidget::cancelImpl()
{
	loadImpl();
}

void AutoReplySettingsWidget::addReplyText(const QString &text)
{
	AutoReplyTextLine *line = new AutoReplyTextLine(text, ui->answersList);
	connect(line, SIGNAL(modified()), SLOT(onModified()));
}

void AutoReplySettingsWidget::onModified()
{
	setModified(true);
}

void AutoReplySettingsWidget::on_addButton_clicked()
{
	addReplyText();
	setModified(true);
}

void AutoReplySettingsWidget::on_automaticBox_toggled(bool checked)
{
	ui->stackedWidget->setCurrentIndex(checked ? 1 : 0);
}
