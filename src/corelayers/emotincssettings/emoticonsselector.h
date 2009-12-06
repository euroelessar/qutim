#ifndef EMOTICONSSELECTOR_H
#define EMOTICONSSELECTOR_H
#include <libqutim/settingswidget.h>
#include "emoticonssettings.h"

namespace Ui {
class emoticonsSelector;
}

using namespace qutim_sdk_0_3;

class EmoticonsSelector : public SettingsWidget
{
	Q_OBJECT
public:
	EmoticonsSelector();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();	
    virtual ~EmoticonsSelector();
private slots:
	void currentIndexChanged(const QString &text);
	void clearEmoticonsPreview();
private:
	Ui::emoticonsSelector *ui;
	QWidgetList m_active_emoticons;
	QString m_selected_theme;
};

#endif // EMOTICONSSELECTOR_H
