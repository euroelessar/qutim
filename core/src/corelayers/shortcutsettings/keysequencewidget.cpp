/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "keysequencewidget.h"
#include <qutim/debug.h>
#include <QMouseEvent>

namespace Core
{

    QKeySequence appendToSequence ( const QKeySequence& seq, int keyQt )
    {
        switch ( seq.count() )
        {
            case 0:
                return QKeySequence ( keyQt );
            case 1:
                return QKeySequence ( seq[0], keyQt );
            case 2:
                return QKeySequence ( seq[0], seq[1], keyQt );
            case 3:
                return QKeySequence ( seq[0], seq[1], seq[2], keyQt );
            default:
                return seq;
        }
    }
    
    QString modifierToString(uint modifier)
    {
        QString s;
        if (modifier & Qt::ALT)
            s += "Alt+";        
        if (modifier & Qt::SHIFT)
            s += "Shift+";        
		if (modifier & Qt::CTRL ) 
#if defined(Q_OS_MAC)            
            s += "Command+";
#else
            s += "Ctrl+";
#endif
        if (modifier & Qt::META)
#if defined(Q_WS_WIN)            
            s += "Win+";
#else
            s += "Meta+";
#endif
        return s;
    }
    
    bool isShiftAsModifierAllowed(int keyQt)
    {
        // Shift only works as a modifier with certain keys. It's not possible
        // to enter the SHIFT+5 key sequence for me because this is handled as
        // '%' by qt on my keyboard.
        // The working keys are all hardcoded here :-(
        if (keyQt >= Qt::Key_F1 && keyQt <= Qt::Key_F35)
            return true;

        if (QChar(keyQt).isLetter())
            return true;

        switch (keyQt) {
            case Qt::Key_Return:
            case Qt::Key_Space:
            case Qt::Key_Backspace:
            case Qt::Key_Escape:
            case Qt::Key_Print:
            case Qt::Key_ScrollLock:
            case Qt::Key_Pause:
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
            case Qt::Key_Insert:
            case Qt::Key_Delete:
            case Qt::Key_Home:
            case Qt::Key_End:
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Left:
            case Qt::Key_Right:
                return true;

            default:
                return false;
        }
    }    

    KeySequenceWidget::KeySequenceWidget ( QWidget* parent ) :
            QPushButton ( parent )
    {
        connect(&m_modifier_timeout,SIGNAL(timeout()),SLOT(doneRecording()));
        //setAutoRepeat(false);
    }

    QKeySequence KeySequenceWidget::sequence() const
    {
        return m_sequence;
    }

    bool KeySequenceWidget::event ( QEvent* e )
    {
        if (m_is_recording && e->type() == QEvent::KeyPress) {
            keyPressEvent(static_cast<QKeyEvent *>(e));
            return true;
        }

        // The shortcut 'alt+c' ( or any other dialog local action shortcut )
        // ended the recording and triggered the action associated with the
        // action. In case of 'alt+c' ending the dialog.  It seems that those
        // ShortcutOverride events get sent even if grabKeyboard() is active.
        if (m_is_recording && e->type() == QEvent::ShortcutOverride) {
            e->accept();
            return true;
        }

        return QPushButton::event(e);
    }

    void KeySequenceWidget::keyReleaseEvent ( QKeyEvent* e )
    {
        if (e->key() == -1) {
            // ignore garbage, see keyPressEvent()
            return;
        }

        if (!m_is_recording)
            return QPushButton::keyReleaseEvent(e);

        e->accept();

        uint newModifiers = e->modifiers() & (Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META);

        //if a modifier that belongs to the shortcut was released...
        if ((newModifiers & m_modifier_keys) < m_modifier_keys) {
            m_modifier_keys = newModifiers;
            updateShortcutDisplay();
            updateModifierTimeout();
        }   
    }

    void KeySequenceWidget::keyPressEvent ( QKeyEvent* e )
    {
        int key = e->key();
        if ( key == -1 )
        {
            // Qt sometimes returns garbage keycodes, I observed -1, if it doesn't know a key.
            // We cannot do anything useful with those (several keys have -1, indistinguishable)
            // and QKeySequence.toString() will also yield a garbage string.
            return;
        }

        uint newModifiers = e->modifiers() & ( Qt::SHIFT | Qt::CTRL | Qt::ALT | Qt::META );
        
        //don't have the return or space key appear as first key of the sequence when they
        //were pressed to start editing - catch and them and imitate their effect
        if (!m_is_recording && ((key == Qt::Key_Return || key == Qt::Key_Space)))
        {
            startRecording();
            m_modifier_keys = newModifiers;
            updateShortcutDisplay();
            return;
        }

        // We get events even if recording isn't active.
        if (!m_is_recording)
            return QPushButton::keyPressEvent (e);

        e->accept();
        m_modifier_keys = newModifiers;
        
        switch (key)
        {
            case Qt::Key_AltGr: //or else we get unicode salad
                return;
            case Qt::Key_Shift:
            case Qt::Key_Control:
            case Qt::Key_Alt:
            case Qt::Key_Meta:
            case Qt::Key_Menu: //unused (yes, but why?)
                updateModifierTimeout();
                updateShortcutDisplay();
                break;
            default:
                // We now have a valid key press.
                if (key)
                {
                    if ((key == Qt::Key_Backtab) && (m_modifier_keys & Qt::SHIFT))
                    {
                        key = Qt::Key_Tab | m_modifier_keys;
                    }
                    else if (isShiftAsModifierAllowed(key)) {
                        key |= m_modifier_keys;
                    }                    
                    else
                        key |= (m_modifier_keys & ~Qt::SHIFT);

                    if ( m_key == 0 ) {
                        m_sequence = QKeySequence (key);
                    }
                    else {
                        m_sequence = appendToSequence (m_sequence, key);
                    }
                    m_key++;
                    if ((m_key >= 2)) {
                        doneRecording();
                        return;
                    }
                    updateModifierTimeout();
                    updateShortcutDisplay();
                }
        }
    }


    void KeySequenceWidget::captureSequence()
    {
        startRecording();
    }

    void KeySequenceWidget::clearSequence()
    {

    }

    void KeySequenceWidget::updateShortcutDisplay()
    {
        //empty string if no non-modifier was pressed
        QString s = m_sequence.toString ( QKeySequence::NativeText );
        s.replace ( '&', QLatin1String ( "&&" ) );

        if (m_is_recording)
        {
            if (m_modifier_keys)
            {
                if (!s.isEmpty())
                    s.append ( "," );
                QString modifiers_string = modifierToString(m_modifier_keys);
                if (!modifiers_string.isEmpty())
                    s += modifiers_string;

            }
            else if ( m_key == 0 )
            {
                s = QT_TRANSLATE_NOOP ( "KeySequenceWidget","What the user inputs now will be taken as the new shortcut" );
            }
            //make it clear that input is still going on
            s.append ( " ..." );
        }

        if ( s.isEmpty() )
        {
            s = QT_TRANSLATE_NOOP ( "KeySequenceWidget", "No shortcut defined" );
        }

        s.prepend ( ' ' );
        s.append ( ' ' );
        setText ( s );
    }

    void KeySequenceWidget::startRecording()
    {
        m_key = 0;
        m_modifier_keys = 0;
        m_old_sequence = m_sequence;   
        m_sequence = QKeySequence();
        m_is_recording = true;
        grabKeyboard();

        if ( !QWidget::keyboardGrabber() )
        {
            qWarning() << "Failed to grab the keyboard! Most likely qt's nograb option is active";
        }

        setDown (true);
        updateShortcutDisplay();
    }

    void KeySequenceWidget::doneRecording()
    {
        m_is_recording = false;
        releaseKeyboard();
        setDown (false);

		// 		if (validate && !q->isKeySequenceAvailable(keySequence)) {
		// 			// The sequence had conflicts and the user said no to stealing it
		// 			keySequence = oldKeySequence;
		// 		} else {
		// 			emit q->keySequenceChanged(keySequence);
		// 		}

        updateShortcutDisplay();
    }

    void KeySequenceWidget::updateModifierTimeout()
    {
        if (m_key != 0 && !m_modifier_keys) {
            // No modifier key pressed currently. Start the timout
            m_modifier_timeout.start(3000);
        } else {
            // A modifier is pressed. Stop the timeout
            m_modifier_timeout.stop();
        }
    }
    
    void KeySequenceWidget::setSequence ( const QKeySequence& sequence )
    {
        if (!m_is_recording)
            m_old_sequence = m_old_sequence;

        m_sequence = sequence;
        doneRecording();
    }
    

}

