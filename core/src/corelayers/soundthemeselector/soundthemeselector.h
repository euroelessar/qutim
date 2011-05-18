#ifndef SOUNDTHEMESELECTOR_H
#define SOUNDTHEMESELECTOR_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Ui {
class SoundThemeSelector;
}
namespace qutim_sdk_0_3
{
class SoundTheme;
}
class QStandardItemModel;
class QModelIndex;
namespace Core
{
using namespace qutim_sdk_0_3;

class SoundThemeSelector : public SettingsWidget
{
	Q_OBJECT

public:
	explicit SoundThemeSelector();
	~SoundThemeSelector();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
protected:
	void changeEvent(QEvent *e);
private slots:
	void currentIndexChanged(const QString &text);
	void onClicked(const QModelIndex &index);
private:
	void fillModel(const SoundTheme &theme);
	Ui::SoundThemeSelector *ui;
	QStandardItemModel *m_model;
};

}
#endif // SOUNDTHEMESELECTOR_H
