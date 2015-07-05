#ifndef AUTOREPLYBUTTONGENERATOR_H
#define AUTOREPLYBUTTONGENERATOR_H

#include <qutim/actiongenerator.h>

class AutoReplyPlugin;

class AutoReplyButtonGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	AutoReplyButtonGenerator(AutoReplyPlugin *plugin);

protected:
	QObject *generateHelper() const;

private:
	AutoReplyPlugin *m_plugin;
};

#endif // AUTOREPLYBUTTONGENERATOR_H
