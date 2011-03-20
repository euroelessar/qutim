#ifndef ICONSLOADERIMPL_H
#define ICONSLOADERIMPL_H

#include <qutim/iconloader.h>
#include <qutim/settingswidget.h>
#include <QComboBox>

using namespace qutim_sdk_0_3;

namespace Core
{
class IconLoaderSettings : public SettingsWidget
{
	Q_OBJECT
public:
	IconLoaderSettings();
	
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
protected slots:
	void onCurrentIndexChanged(int index);
private:
	QComboBox *m_box;
	int m_index;
};

class IconLoaderImpl : public IconLoader
{
	Q_OBJECT
public:
	IconLoaderImpl();
	virtual QIcon loadIcon(const QString &name);
	virtual QMovie *loadMovie(const QString &name);
	virtual QString iconPath(const QString &name, uint iconSize);
	virtual QString moviePath(const QString &name, uint iconSize);
public slots:
	void onSettingsChanged();
	void initSettings();
};
}

#endif // ICONSLOADERIMPL_H
