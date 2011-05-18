/*#include "JumpListTask.h"
#include <QAction>
#include <QIcon>

JumpListTask::JumpListTask(QObject *parent)
	: QObject(parent)
{

}

void JumpListTask::setAppPath(const QString &path)
{
	m_appPath = path;
}

void JumpListTask::setTitle(const QString &title)
{
	m_title = title;
}

void JumpListTask::setTooltip(const QString &tip)
{
	m_description = tip;
}

void JumpListTask::setIcon(const QPixmap &pixmap)
{
	m_icon = pixmap;
}

void JumpListTask::setIcon(const QIcon &icon)
{
	setIcon(icon.pixmap(16, 16));
}

QString JumpListTask::title()
{
	return m_title;
}

QString JumpListTask::tooltip()
{
	return m_description;
}

QString JumpListTask::appPath()
{
	return m_appPath;
}

QPixmap JumpListTask::icon()
{
	return m_icon;
}
*/
