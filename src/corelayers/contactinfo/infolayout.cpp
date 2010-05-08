#include "infolayout.h"
#include <QSpacerItem>

namespace Core
{

AbstractInfoLayout::AbstractInfoLayout(QWidget *parent) :
	QGridLayout(parent), m_row(0)
{
}

AbstractInfoLayout::~AbstractInfoLayout()
{
}

void AbstractInfoLayout::addSpacer()
{
	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout::addItem(spacer, m_row++, 0);
}

}
