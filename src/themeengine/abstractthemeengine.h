#ifndef ABSTRACTTHEMEENGINE_H_
#define ABSTRACTTHEMEENGINE_H_

#include <QWidget>
#include "qskinobject.h"

class AbstractThemeEngine
{
public:
	static AbstractThemeEngine &instance();
	void setCLBorder(QWidget *cl_widget);
	void stopSkiningCl();
	void loadProfile(const QString &profile_name);
	void reloadContent();
private:
	AbstractThemeEngine();
	virtual ~AbstractThemeEngine();
	QSkinObject *m_border_object;
	QString m_profile_name;
	QString m_border_theme_path;
};

#endif /*ABSTRACTTHEMEENGINE_H_*/
