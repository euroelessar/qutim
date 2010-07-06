#include "profilelistwidget.h"
#include <QtGui/QGridLayout>
#include <QtGui/QSpacerItem>

namespace Core
{
ProfileListWidget::ProfileListWidget(const QString &id, const QString &configDir)
{
	QGridLayout *gridLayout = new QGridLayout();
	photoLabel = new QLabel();
	QLabel *idLabel = new QLabel();
	passwordEdit = new QLineEdit();
	loginButton = new QPushButton();
	QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	connect(loginButton, SIGNAL(clicked()), this, SLOT(wantLogin()));
	connect(passwordEdit, SIGNAL(returnPressed()), this, SLOT(wantLogin()));

	photoLabel->setMinimumSize(QSize(64, 64));
	photoLabel->setMaximumSize(QSize(64, 64));
	passwordEdit->setEchoMode(QLineEdit::Password);
	loginButton->setMinimumSize(QSize(22, 22));
	loginButton->setMaximumSize(QSize(22, 22));
	loginButton->setFocusPolicy(Qt::NoFocus);
	gridLayout->addWidget(photoLabel, 0, 0, 3, 1);
	gridLayout->addWidget(idLabel, 0, 1, 1, 1);
	gridLayout->addWidget(passwordEdit, 2, 1, 1, 1);
	gridLayout->addWidget(loginButton, 2, 2, 1, 1);
	gridLayout->addItem(verticalSpacer, 1, 1, 1, 1);
	gridLayout->setContentsMargins(0, 0, 5, 5);

	setLayout(gridLayout);

	QPixmap photo(configDir + "/avatars/profilephoto");
	if (photo.isNull())
		photo.load(":/icons/qutim_64.png");
	photoLabel->setPixmap(photo);
	idLabel->setText("<h2>" + id + "</h2>");
	loginButton->setText(">");

	activate(false);
}

ProfileListWidget::~ProfileListWidget()
{
}

void ProfileListWidget::activate(bool a)
{
	passwordEdit->setVisible(a);
	loginButton->setVisible(a);
	photoLabel->setEnabled(a);
	if (a)
		passwordEdit->setFocus(Qt::OtherFocusReason);
}

void ProfileListWidget::wantLogin()
{
	emit submit(passwordEdit->text());
}
}
