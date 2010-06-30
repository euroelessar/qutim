#ifndef PROFILELISTWIDGET_H
#define PROFILELISTWIDGET_H

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

namespace qutim_sdk_0_3
{
	class Config;
}

namespace Core
{
class ProfileListWidget : public QWidget
{
	Q_OBJECT

public:
	ProfileListWidget(const QString &id, const QString &configDir);
	~ProfileListWidget();

	void activate(bool a);

signals:
	void submit(const QString &password);

private slots:
	void wantLogin();

private:
	QLabel *photoLabel;
	QLineEdit *passwordEdit;
	QPushButton *loginButton;
};
}

#endif // PROFILELISTWIDGET_H
