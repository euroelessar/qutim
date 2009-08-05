#ifndef CHATWINDOWSTYLEOUTPUT_H_
#define CHATWINDOWSTYLEOUTPUT_H_

#include <QObject>
#include "chatwindowstyle.h"

class ChatWindowStyleOutput
{
public:
	/*
	  constructor, _styleName - name of the style to use, it have to be same to directory name
	  _variantName - name of the CSS file to use, don't write .css
	*/
	ChatWindowStyleOutput(const QString &_styleName, const QString &_variantName);
	~ChatWindowStyleOutput();

	/*
	  passes valriants QHash (list of all variants) from lower class
	*/
	AdiumMessageStyle::ChatWindowStyle::StyleVariants getVariants() const;

	/*
	  sets variant to use, it's used for setting
	*/
	void setVariant(const QString &_variantName);
	
	/*
	  list paths, where html linked data can be found
	*/
	QStringList getPaths();

	/*
	  creats a html skeleton. Future messages will be added to it
	  skeleton consist of styles, header and footer
	  it has a mark as well. before this mark new messages should be added
	  _chatName - name of chat, example "Weekends plans"
	  _ownerName - name or nickname of program owner
	  _partnerName - name or nicname of chating partner
	  _ownerIconPath & _partnerIconPath - path to image files representing ppl,
	                                      "" uses style provided pictures
	  _time - it's time when the chat has started
	*/
	QString makeSkeleton(const QString &_chatName, const QString &_ownerName, 
						 const QString &_partnerName, const QString &_ownerIconPath,
						 const QString &_partnerIconPath, const QDateTime &datetime, const QString &_time);
	/*
	  changes keywords to message atributes in html
	  _name - sender's nickname
	  _message - message text
	  _direction - "true" if it was send, "false" if it was recived
	  _time - time at witch message was send
	  _avatarPath - path to avatar, if "" uses style provided picture
	  _aligment - "true" if left-to-right, "false" if right-to-left
	  _senderID - sender's ID
	  _servese - protocol used to send a message
	  _sameSender - "true" - same sender, "false" - message is send by another person
	  _history - if message was sent to offline, or gotten from history
	*/
	QString makeMessage(const QString &_name, const QString &_message,
						const bool &_direction, const QDateTime &datetime, const QString &_time, const QString &_avatarPath,
						const bool &_aligment, const QString &_senderID, const QString &_service, 
						const bool &_sameSender, bool _history);
	/*
	  changes keywords to action atributes in html like "Bob is writing on the desk"
	  _name - sender's nickname
	  _message - message text
	  _direction - "true" if it was send, "false" if it was recived
	  _time - time at witch message was send
	  _avatarPath - path to avatar, if "" uses style provided picture
	  _aligment - "true" if left-to-right, "false" if right-to-left
	  _senderID - sender's ID
	  _servese - protocol used to send a message
	*/
	QString makeAction(const QString &_name, const QString &_message,
						const bool &_direction, const QDateTime &datetime, const QString &_time, const QString &_avatarPath,
						const bool &_aligment, const QString &_senderID, const QString &_service);

	/*
	  It is used for displaying system and user messages like "user gone offline",
	  "Marina is now away", "You are being ignored" etc.
	  _message - message by it self to be shown
	  _time - timestamp
	*/
	QString makeStatus(const QString &_message, const QDateTime &datetime, const QString &_time);

	/*
	  for degubing purpose, must be deleted before release
	*/
	QString getMainCSS();
	QString getVariantCSS();
	void preparePage(QWebPage *page) const;
private:
	/*
	  makes html code from plaint text
	*/
	QString makeHTML(const QString &_sourceText);

	QString findEmail(const QString &_sourceHTML);

	QString findWebAddress(const QString &_sourceHTML);

private:
	/*
	  style used for output generation
	*/
	AdiumMessageStyle::ChatWindowStyle *styleUsed;

	/*
	  remembers current variant name
	*/
	QString variantUsedName;
};

#endif /*CHATWINDOWSTYLEOUTPUT_H_*/
