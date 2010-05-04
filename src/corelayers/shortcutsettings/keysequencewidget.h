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
