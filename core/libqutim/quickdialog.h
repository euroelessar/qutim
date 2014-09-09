#ifndef QUTIM_SDK_0_3_QUICKDIALOG_H
#define QUTIM_SDK_0_3_QUICKDIALOG_H

#include <QObject>
#include <QString>
#include <QScopedPointer>

namespace qutim_sdk_0_3 {

class QuickDialogPrivate;

class QuickDialog : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(QuickDialog)
	Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
public:
	QuickDialog(const QString &name);
	~QuickDialog();

	void setVisible(bool visible);
	bool isVisible();

public slots:
	void show();
	void hide();

signals:
	void visibleChanged(bool visible);

private slots:
	void implementationVisibileChanged();

private:
	QScopedPointer<QuickDialogPrivate> d_ptr;
};

} // namespace qutim_sdk_0_3

#endif // QUTIM_SDK_0_3_QUICKDIALOG_H
