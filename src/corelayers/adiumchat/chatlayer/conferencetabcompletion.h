/*
 * Copyright (C) 2001-2008 Justin Karneges, Martin Hostettler
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Generic tab completion support code.

#ifndef CONFERENCETABCOMPLETION_H_
#define CONFERENCETABCOMPLETION_H_

#include <QObject>
#include <QPlainTextEdit>
#include "chatsessionimpl.h"
namespace Core
{

	namespace AdiumChat
	{

		class ConfTabCompletion : public QObject
		{
			Q_OBJECT

		public:
			ConfTabCompletion(QObject *parent = 0);
			virtual ~ConfTabCompletion();

			void setTextEdit(QPlainTextEdit* conferenceTextEdit);
			QPlainTextEdit* getTextEdit();

			virtual void reset();
			void tryComplete();

			void setup(QString str, int pos, int &start, int &end);
			QStringList possibleCompletions();
			QStringList allChoices(QString &guess);

			QStringList getUsers();

			void setChatSession(ChatSessionImpl  *session);
			void setLastReferrer(QString last_referrer);
		private:
			QString nickSep;

			QString toComplete_;
			bool atStart_;

			virtual void highlight(bool set);
			QColor highlight_;

			QString longestCommonPrefix(QStringList list);
			QString suggestCompletion(bool *replaced);
			virtual bool eventFilter(QObject* , QEvent* );

			void moveCursorToOffset(QTextCursor &cur, int offset, QTextCursor::MoveMode mode = QTextCursor::MoveAnchor);

			enum TypingStatus {
				Typing_Normal,
				Typing_TabPressed,	// initial completion
				Typing_TabbingCompletions, // switch to tab through multiple
				Typing_MultipleSuggestions
			};

			QTextCursor replacementCursor_;
			TypingStatus typingStatus_;
			QStringList suggestedCompletion_;
			int  suggestedIndex_;

			QPointer<QPlainTextEdit> textEdit_;
			ChatSessionImpl *chat_session_;
			QString last_referrer_;
		};

	}
}
#endif /* CONFERENCETABCOMPLETION_H_ */
