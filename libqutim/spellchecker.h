/****************************************************************************
 *  spellchecker.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3 {

/**
  The SpellChecker class is used for spell checking.
 */
// TODO: do we need languages() and setLanguage() methods?
class LIBQUTIM_EXPORT SpellChecker : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "SpellChecker")
	Q_DISABLE_COPY(SpellChecker)
public:
	/**
	   Destroyes the object.
	*/
	virtual ~SpellChecker();
	/**
	  Returns whether the \a word is spelled correctly.
	*/
	virtual bool isCorrect(const QString &word) const = 0;
	/**
	  Returns whether the \a word is misspelled.
	*/
	bool isMisspelled(const QString &word) const { return !isCorrect(word); }
	/**
	  Returns a list of spelling suggestions for the \a word.
	*/
	virtual QStringList suggest(const QString &word) const = 0;
	/**
	  Stores the \a word to the user personal dictionary.
	*/
	virtual void store(const QString &word) const = 0;
	/**
	  Stores user defined \a good replacement for the \a bad word.
	*/
	virtual void storeReplacement(const QString &bad, const QString &good) = 0;
	/**
	  Returns the CheckSpeller object.
	*/
	static SpellChecker *instance();
signals:
	/**
	  The signal is sent when the dictionary has been changed.
	*/
	void dictionaryChanged();
protected:
	SpellChecker();
	virtual void virtual_hook(int id, void *data);
};

} // namespace qutim_sdk_0_3

#endif // SPELLCHECKER_H
