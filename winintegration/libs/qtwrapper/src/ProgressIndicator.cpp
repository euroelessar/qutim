#include "ProgressIndicator.h"
#include "../../apilayer/src/ApiProgressIndicator.h"
#include <QWidget>

ProgressIndicator::ProgressIndicator(QWidget *w, QObject *parent)
	: QObject(parent)
{
	m_window = w;
}

void ProgressIndicator::changeWindow(QWidget* w)
{
	m_window = w;
}

void ProgressIndicator::setState(ProgressStates state)
{
	SetProgressState(m_window->winId(), state);
}

void ProgressIndicator::setValue(unsigned val, unsigned max)
{
	SetProgressValEx(m_window->winId(), val, max);
}

void ProgressIndicator::clear()
{
	setState(PS_None);
}

void ProgressIndicator::setValue(QWidget *w, unsigned val, unsigned max)
{
	ProgressIndicator i(w);
	i.setValue(val, max);
}

void ProgressIndicator::setState(QWidget *w, ProgressStates state)
{
	ProgressIndicator i(w);
	i.setState(state);
}
