#ifndef ICON_H
#define ICON_H

#include <QIcon>

class Icon : public QIcon
{
public:
	// Qt 4.6 has ability for getting icons from system themes
	// but has no one for getting client-specific icons like
	// status icons and client icons (except installed clients)
	// so we create our own imlp of icon engine, which has wrapper
	// around QIcon::fromTheme
	enum Type { System, Status };
    Icon();
};

#endif // ICON_H
