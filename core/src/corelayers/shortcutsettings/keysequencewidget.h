/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#ifndef KEYSEQUENCEWIDGET_H
#define KEYSEQUENCEWIDGET_H

#include <QPushButton>
#include <QTimer>

namespace Core
{

	class KeySequenceWidget : public QPushButton
	{
		Q_OBJECT
	public:
		KeySequenceWidget(QWidget* parent = 0);
		QKeySequence sequence() const;
		void setSequence(const QKeySequence &sequence);
	public slots:
		void captureSequence();
		void clearSequence();
	private slots:
		void updateShortcutDisplay();
		void startRecording();
		void doneRecording();
	protected:
		virtual void keyReleaseEvent(QKeyEvent* e);
		virtual void keyPressEvent ( QKeyEvent* e);
		virtual bool event ( QEvent* e );
	private:
		void updateModifierTimeout();
		uint m_key;
		uint m_modifier_keys;
		bool m_is_recording;
		QKeySequence m_sequence;
		QKeySequence m_old_sequence;
		QTimer m_modifier_timeout;
	};

}
#endif // KEYSEQUENCEWIDGET_H

