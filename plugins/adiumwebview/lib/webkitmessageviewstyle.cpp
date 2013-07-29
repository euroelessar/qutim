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

#include "webkitmessageviewstyle.h"
#include "webkitcolorsadditions_p.h"
#include <QColor>
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include <QVariantMap>
#include <QUrl>
#include <QTextDocument>
#include <qutim/config.h>
#include <qutim/conference.h>
#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/thememanager.h>

using namespace qutim_sdk_0_3;

class WebKitMessageViewStylePrivate
{
public:
	int styleVersion;
	QVariantMap config;
	QString stylePath;
	QString activeVariant;
	QString customStyle;
	
	//Templates
	QString headerHTML;
	QString footerHTML;
	QString baseHTML;
	QString contentHTML;
	QString contentInHTML;
	QString nextContentInHTML;
	QString	contextInHTML;
	QString	nextContextInHTML;
	QString	contentOutHTML;
	QString	nextContentOutHTML;
	QString	contextOutHTML;
	QString	nextContextOutHTML;
	QString	statusHTML;
	QString	fileTransferHTML;
	QString	topicHTML;
	QString actionHTML;
	QString actionInHTML;
	QString actionOutHTML;
	
	//Style settings
	bool allowsCustomBackground;
	bool transparentDefaultBackground;
	bool allowsUserIcons;
	bool usingCustomTemplateHTML;
	
	bool checkedSenderColors;
	QStringList validSenderColors;
	
	bool checkedVariants;
	QStringList allowedVariants;
	
	//Behavior
	QString timeStampFormatter;
	QString nameFormat;
	bool useCustomNameFormat;
	bool showUserIcons;
	bool showHeader;
	bool combineConsecutive;
	bool allowTextBackgrounds;
	bool allowsColors;
	bool showIncomingFonts;
	bool showIncomingColors;
	WebKitMessageViewStyle::WebkitBackgroundType customBackgroundType;
	QString customBackgroundPath;
	QColor customBackgroundColor;
	QImage userIconMask;
};


/*!
 *	@brief Key used to retrieve the user's icon
 */
#define KEY_WEBKIT_USER_ICON 				"WebKitUserIconPath"

/*!
 *	@brief Key used to retrieve the default font family
 */
#define KEY_WEBKIT_DEFAULT_FONT_FAMILY		"DefaultFontFamily"

/*!
 *	@brief Key used to retrieve the default font size
 */
#define KEY_WEBKIT_DEFAULT_FONT_SIZE		"DefaultFontSize"

/*!
 *	@brief Key used to retrieve the mask for the user's icon
 */
#define KEY_WEBKIT_USER_ICON_MASK			"ImageMask"

//
#define LEGACY_VERSION_THRESHOLD		3	//Styles older than this version are considered legacy

//
#define KEY_WEBKIT_VERSION				"MessageViewVersion"

//BOM scripts for appending content.
#define APPEND_MESSAGE_WITH_SCROLL		"checkIfScrollToBottomIsNeeded(); appendMessage(\"%1\"); scrollToBottomIfNeeded();"
#define APPEND_NEXT_MESSAGE_WITH_SCROLL	"checkIfScrollToBottomIsNeeded(); appendNextMessage(\"%1\"); scrollToBottomIfNeeded();"
#define APPEND_MESSAGE					"appendMessage(\"%1\");"
#define APPEND_NEXT_MESSAGE				"appendNextMessage(\"%1\");"
#define APPEND_MESSAGE_NO_SCROLL		"appendMessageNoScroll(\"%1\");"
#define	APPEND_NEXT_MESSAGE_NO_SCROLL	"appendNextMessageNoScroll(\"%1\");"
#define REPLACE_LAST_MESSAGE			"replaceLastMessage(\"%1\");"

#define TOPIC_MAIN_DIV					"<div id=\"topic\"></div>"
// We set back, when the user finishes editing, the correct topic, which wipes out the existance of the span before. We must undo the dbl click action.
#define TOPIC_INDIVIDUAL_WRAPPER		"<span id=\"topicEdit\" ondblclick=\"this.setAttribute('contentEditable', true); this.focus();\"" \
	" onkeydown=\"if (event.keyCode == 13 && !event.shiftKey) { event.preventDefault(); this.setAttribute('contentsEditable', false); client.setTopic(); }\">%1</span>"
#define ACTION_SPAN "<span class='actionMessageUserName'>%1</span><span class='actionMessageBody'>%2</span>"

namespace qutim_sdk_0_3 {
LIBQUTIM_EXPORT QString convertTimeDate(const QString &macFormat, const QDateTime &datetime);
LIBQUTIM_EXPORT QString &validateCpp(QString &text);
}


static QString urlFromFilePath(const QString &filePath)
{
	if (filePath.isEmpty())
		return QString();
	return QUrl::fromLocalFile(filePath).toString();
}

static QString escapeString(const QString &text)
{
	return text.toHtmlEscaped().replace(QLatin1Char('%'), QLatin1String("&#37;"));
}

WebKitMessageViewStyle::WebKitMessageViewStyle() : d_ptr(new WebKitMessageViewStylePrivate)
{
	Q_D(WebKitMessageViewStyle);
	d->showUserIcons = true;
	d->showHeader = true;
	d->timeStampFormatter = "HH:mm:ss";
	d->customBackgroundType = BackgroundNormal;
}

WebKitMessageViewStyle::~WebKitMessageViewStyle()
{
}

QString WebKitMessageViewStyle::noVariantName() const
{
	Q_D(const WebKitMessageViewStyle);
	QString noVariantName = d->config.value(QLatin1String("DisplayNameForNoVariant")).toString();
	return noVariantName.isEmpty() ? QObject::tr("Normal") : noVariantName;
}

QString WebKitMessageViewStyle::defaultVariant() const
{
	Q_D(const WebKitMessageViewStyle);
	return d->styleVersion < 3 ? noVariantName() : d->config.value(QLatin1String("DefaultVariant")).toString();
}

QString WebKitMessageViewStyle::activeVariantPath() const
{
	Q_D(const WebKitMessageViewStyle);
	if (d->styleVersion < 3 && d->activeVariant == noVariantName())
		return QLatin1String("main.css");
	return QString::fromLatin1("Variants/%1.css").arg(d->activeVariant);
}

QStringList WebKitMessageViewStyle::validSenderColors()
{
	Q_D(WebKitMessageViewStyle);
	if (!d->checkedSenderColors) {
		QString senderColorsFile = loadResourceFile(QLatin1String("SenderColors.txt"), QLatin1String("Incoming"));
		d->validSenderColors = senderColorsFile.split(QLatin1Char(':'));
		d->checkedSenderColors = true;
	}
	return d->validSenderColors;
}

void WebKitMessageViewStyle::reloadStyle()
{
	Q_D(WebKitMessageViewStyle);
	releaseResources();
	
	//Default behavior
	d->allowTextBackgrounds = true;
	
	/* Our styles are versioned so we can change how they work without breaking compatibility.
	 *
	 * Version 0: Initial Webkit Version
	 * Version 1: Template.html now handles all scroll-to-bottom functionality.  It is no longer required to call the
	 *            scrollToBottom functions when inserting content.
	 * Version 2: No significant changes
	 * Version 3: main.css is no longer a separate style, it now serves as the base stylesheet and is imported by default.
	 *            The default variant is now a separate file in /variants like all other variants.
	 *			  Template.html now includes appendMessageNoScroll() and appendNextMessageNoScroll() which behave
	 *				the same as appendMessage() and appendNextMessage() in Versions 1 and 2 but without scrolling.
	 * Version 4: Template.html now includes replaceLastMessage()
	 *            Template.html now defines actionMessageUserName and actionMessageBody for display of /me (actions).
	 *				 If the style provides a custom Template.html, these classes must be defined.
	 *				 CSS can be used to customize the appearance of actions.
	 *			  HTML filters in are now supported in Adium's content filter system; filters can assume Version 4 or later.
	 */
	d->styleVersion = d->config.value(QLatin1String(KEY_WEBKIT_VERSION)).toInt();
	
	d->activeVariant = defaultVariant();
	if (d->activeVariant.isEmpty())
		d->activeVariant = variants().value(0);
	
	loadTemplates();
	
	//Style flags
	d->allowsCustomBackground = !d->config.value(QLatin1String("DisableCustomBackground")).toBool();
	d->transparentDefaultBackground = d->config.value(QLatin1String("DefaultBackgroundIsTransparent")).toBool();
	
	d->combineConsecutive = !d->config.value(QLatin1String("DisableCombineConsecutive")).toBool();
	
	d->allowsUserIcons = d->config.value(QLatin1String("ShowsUserIcons"), true).toBool();
	
	//User icon masking
	QString tmpName = d->config.value(QLatin1String(KEY_WEBKIT_USER_ICON_MASK)).toString();
	Q_UNUSED(tmpName);
//	d->userIconMask = 
//	NSString *tmpName = [styleBundle objectForInfoDictionaryKey:KEY_WEBKIT_USER_ICON_MASK];
//	if (tmpName) userIconMask = [[NSImage alloc] initWithContentsOfFile:[stylePath stringByAppendingPathComponent:tmpName]];
	
	d->allowsColors = d->config.value(QLatin1String("AllowTextColors"), true).toBool();
}

QStringList WebKitMessageViewStyle::variants()
{
	Q_D(WebKitMessageViewStyle);
	if (!d->checkedVariants) {
		d->allowedVariants.clear();
		QDir dir = d->stylePath + QLatin1String("/Variants");
		QStringList variants = dir.entryList(QStringList(QLatin1String("*.css")));
		const QRegExp regexp(QLatin1String("\\.css$"));
		variants.replaceInStrings(regexp, QString());
		if (d->styleVersion < 3)
			d->allowedVariants.append(noVariantName());
		d->allowedVariants.append(variants);
	}
	return d->allowedVariants;
}

QString WebKitMessageViewStyle::activeVariant() const
{
	Q_D(const WebKitMessageViewStyle);
	return d->activeVariant;
}

void WebKitMessageViewStyle::setActiveVariant(const QString &variant)
{
	Q_D(WebKitMessageViewStyle);
	d->activeVariant = variant;
}

bool WebKitMessageViewStyle::allowsCustomBackground() const
{
	return d_func()->allowsCustomBackground;
}

bool WebKitMessageViewStyle::transparentDefaultBackground() const
{
	return d_func()->transparentDefaultBackground;
}

bool WebKitMessageViewStyle::allowsUserIcons() const
{
	return d_func()->allowsUserIcons;
}

bool WebKitMessageViewStyle::hasHeader() const
{
	return !d_func()->headerHTML.isEmpty();
}

bool WebKitMessageViewStyle::hasTopic() const
{
	return !d_func()->topicHTML.isEmpty();
}

bool WebKitMessageViewStyle::hasAction() const
{
	return !d_func()->actionInHTML.isEmpty();
}

QString WebKitMessageViewStyle::defaultFontFamily() const
{
	return d_func()->config.value(QLatin1String("DefaultFontFamily")).toString();
}

int WebKitMessageViewStyle::defaultFontSize() const
{
	return d_func()->config.value(QLatin1String("DefaultFontSize"), -1).toInt();
}

bool WebKitMessageViewStyle::showUserIcons() const
{
	return d_func()->showUserIcons;
}

void WebKitMessageViewStyle::setShowUserIcons(bool showUserIcons)
{
	d_func()->showUserIcons = showUserIcons;
}

bool WebKitMessageViewStyle::showHeader() const
{
	return d_func()->showHeader;
}

void WebKitMessageViewStyle::setShowHeader(bool showHeader)
{
	d_func()->showHeader = showHeader;
}

bool WebKitMessageViewStyle::showIncomingFonts() const
{
	return d_func()->showIncomingFonts;
}

void WebKitMessageViewStyle::setShowIncomingFonts(bool showIncomingFonts)
{
	d_func()->showIncomingFonts = showIncomingFonts;
}

bool WebKitMessageViewStyle::showIncomingColors() const
{
	return d_func()->showIncomingColors;
}

void WebKitMessageViewStyle::setShowIncomingColors(bool showIncomingColors)
{
	d_func()->showIncomingColors = showIncomingColors;
}

WebKitMessageViewStyle::WebkitBackgroundType WebKitMessageViewStyle::customBackgroundType() const
{
	return d_func()->customBackgroundType;
}

void WebKitMessageViewStyle::setCustomBackgroundType(WebKitMessageViewStyle::WebkitBackgroundType type)
{
	d_func()->customBackgroundType = type;
}

QString WebKitMessageViewStyle::customBackgroundPath() const
{
	return d_func()->customBackgroundPath;
}

void WebKitMessageViewStyle::setCustomBackgroundPath(const QString &path)
{
	Q_D(WebKitMessageViewStyle);
	if (path.isEmpty())
		d->customBackgroundPath.clear();
	else
		d->customBackgroundPath = QUrl::fromLocalFile(path).toString();
}

QColor WebKitMessageViewStyle::customBackgroundColor() const
{
	return d_func()->customBackgroundColor;
}

void WebKitMessageViewStyle::setCustomBackgroundColor(const QColor &color)
{
	d_func()->customBackgroundColor = color;
}

void WebKitMessageViewStyle::setStylePath(const QString &path)
{
	Q_D(WebKitMessageViewStyle);
	QDir dir = path;
	dir.cd(QLatin1String("Contents"));
	Config cfg(dir.filePath(QLatin1String("Info.plist")));
	d->config = cfg.rootValue().toMap();
	dir.cd(QLatin1String("Resources"));
	d->stylePath = dir.absolutePath() + QLatin1Char('/');
	reloadStyle();
}

void WebKitMessageViewStyle::setCustomStyle(const QString &style)
{
	Q_D(WebKitMessageViewStyle);
	d->customStyle = style;
}

QString WebKitMessageViewStyle::baseTemplateForChat(qutim_sdk_0_3::ChatSession *session)
{
	Q_D(WebKitMessageViewStyle);
	QString templateHTML;

	// If this is a group chat, we want to include a topic.
	// Otherwise, if the header is shown, use it.
	QString headerContent;
	if (d->showHeader) {
		if (qobject_cast<Conference*>(session->unit())) {
			headerContent = QLatin1String(/*chat.supportsTopic ?*/ TOPIC_MAIN_DIV /*: ""*/);
		} else {
			headerContent = d->headerHTML;
		}
	}
	
	//Old styles may be using an old custom 4 parameter baseHTML.  Styles version 3 and higher should
	//be using the bundled (or a custom) 5 parameter baseHTML.
	if ((d->styleVersion < 3) && d->usingCustomTemplateHTML) {
		templateHTML = stringWithFormat(d->baseHTML, QStringList()
		                                << QUrl::fromLocalFile(d->stylePath).toString()
		                                << activeVariantPath()
		                                << headerContent
		                                << d->footerHTML);
	} else {
		templateHTML = stringWithFormat(d->baseHTML, QStringList()
		                                << QUrl::fromLocalFile(d->stylePath).toString()
		                                << QLatin1String(d->styleVersion < 3 ? "" : "@import url( \"main.css\" );")
		                                << activeVariantPath()
		                                << headerContent
		                                << d->footerHTML);
	}

	return fillKeywordsForBaseTemplate(templateHTML, session);
}

QString WebKitMessageViewStyle::scriptForChangingVariant()
{
	return QString::fromLatin1("setStylesheet(\"mainStyle\",\"%1\");").arg(activeVariantPath());
}

QString WebKitMessageViewStyle::scriptForSettingCustomStyle()
{
	Q_D(WebKitMessageViewStyle);
	return QString::fromLatin1("setCustomStylesheet(\"%1\");").arg(d->customStyle);
}

QString WebKitMessageViewStyle::scriptForAppendingContent(const qutim_sdk_0_3::Message &message, bool contentIsSimilar, bool willAddMoreContentObjects, bool replaceLastContent)
{
	Q_D(WebKitMessageViewStyle);
	
	//If combining of consecutive messages has been disabled, we treat all content as non-similar
	
	if (!d->combineConsecutive)
		contentIsSimilar = false;
	
	//Fetch the correct template and substitute keywords for the passed content
	QString newHTML = templateForContent(message, contentIsSimilar);
	QString script;
	
	//BOM scripts vary by style version
	if (!d->usingCustomTemplateHTML && d->styleVersion >= 4) {
		/* If we're using the built-in template HTML, we know that it supports our most modern scripts */
		if (replaceLastContent)
			script = QLatin1String(REPLACE_LAST_MESSAGE);
		else if (willAddMoreContentObjects) {
			script = QLatin1String(contentIsSimilar ? APPEND_NEXT_MESSAGE_NO_SCROLL : APPEND_MESSAGE_NO_SCROLL);
		} else {
			script = QLatin1String(contentIsSimilar ? APPEND_NEXT_MESSAGE : APPEND_MESSAGE);
		}
		
	} else  if (d->styleVersion >= 3) {
		if (willAddMoreContentObjects) {
			script = QLatin1String(contentIsSimilar ? APPEND_NEXT_MESSAGE_NO_SCROLL : APPEND_MESSAGE_NO_SCROLL);
		} else {
			script = QLatin1String(contentIsSimilar ? APPEND_NEXT_MESSAGE : APPEND_MESSAGE);
		}
	} else if (d->styleVersion >= 1) {
		script = QLatin1String(contentIsSimilar ? APPEND_NEXT_MESSAGE : APPEND_MESSAGE);
		
	} else {
		if (d->usingCustomTemplateHTML && message.property("service", false)) {
			/* Old styles with a custom template.html had Status.html files without 'insert' divs coupled 
			 * with a APPEND_NEXT_MESSAGE_WITH_SCROLL script which assumes one exists.
			 */
			script = QLatin1String(APPEND_MESSAGE_WITH_SCROLL);
		} else {
			script = QLatin1String(contentIsSimilar ? APPEND_NEXT_MESSAGE_WITH_SCROLL : APPEND_MESSAGE_WITH_SCROLL);
		}
	}
	
	return script.arg(validateCpp(newHTML));
}

QString &WebKitMessageViewStyle::fillKeywordsForBaseTemplate(QString &inString, qutim_sdk_0_3::ChatSession *session)
{
	Q_D(WebKitMessageViewStyle);
	ChatUnit *unit = session->unit();
	Account *account = unit->account();
	Protocol *protocol = account->protocol();
	
	// FIXME: Should be session->title
	inString.replace(QLatin1String("%chatName%"), escapeString(unit->title()));
	inString.replace(QLatin1String("%sourceName%"), escapeString(account->name()));
	inString.replace(QLatin1String("%destinationName%"), escapeString(unit->id()));
	inString.replace(QLatin1String("%destinationDisplayName%"), escapeString(unit->title()));

	QString iconPath;
	
	inString.replace(QLatin1String("%incomingColor%"),
	                 WebKitColorsAdditions::representedColorForObject(unit->id(), validSenderColors()));
	inString.replace(QLatin1String("%outgoingColor%"),
	                 WebKitColorsAdditions::representedColorForObject(account->name(), validSenderColors()));
	
		
	iconPath = urlFromFilePath(unit->property("avatar").toString());
	if (iconPath.isEmpty()) {
		ChatUnit *contact = unit->upperUnit();
		while (contact && iconPath.isEmpty()) {
			iconPath = contact->property("avatar").toString();
			contact = contact->upperUnit();
		}
	}
	
	inString.replace(QLatin1String("%incomingIconPath%"),
	                 iconPath.isEmpty() ? QString::fromLatin1("incoming_icon.png") : iconPath);

	iconPath = urlFromFilePath(account->property("avatar").toString());
	inString.replace(QLatin1String("%outgoingIconPath%"),
	                 iconPath.isEmpty() ? QString::fromLatin1("outgoing_icon.png") : iconPath);

	// FIXME: Implement protocol.iconPath and protocol.shortDescription
	QString serviceIconPath = QString(); //account.protocol.iconPath;
	QString serviceIconTag = QString::fromLatin1("<img class=\"serviceIcon\" src=\"%1\" alt=\"%2\" title=\"%2\">")
	        .arg(serviceIconPath.isEmpty() ? QString::fromLatin1("outgoing_icon.png") : serviceIconPath, protocol->id() /*shortDescription*/ );
	
	inString.replace(QLatin1String("%serviceIconImg%"), serviceIconTag);
	inString.replace(QLatin1String("%serviceIconPath%"), serviceIconPath);
	inString.replace(QLatin1String("%timeOpened%"), convertTimeDate(d->timeStampFormatter, session->dateOpened()));
	
	//Replaces %time{x}% with a timestamp formatted like x (using NSDateFormatter)
	const QStringMatcher matcher(QLatin1String("%timeOpened{"));
	const int matcherSize = matcher.pattern().size();
	const QStringMatcher endMatcher(QLatin1String("}%"));
	const int endMatcherSize = endMatcher.pattern().size();
	int range = 0;
	do {
		range = matcher.indexIn(inString, range);
		if (range != -1) {
			int endRange = endMatcher.indexIn(inString, range + matcherSize);
			if (endRange != -1) {
				QString timeFormat = inString.mid(range + matcherSize, endRange - range - matcherSize);
				QString time = convertTimeDate(timeFormat, session->dateOpened());
				inString.replace(range, endRange + endMatcherSize - range, time);
				range = range + time.size();
			}
		}
	} while (range != -1);
	
	inString.replace(QLatin1String("%dateOpened%"), session->dateOpened().toString(Qt::SystemLocaleLongDate));
	
	//Background
	{
		QLatin1String bodyBackground("==bodyBackground==");
		range = inString.indexOf(bodyBackground);
		
		if (range != -1) { //a backgroundImage tag is not required
			QString bodyTag;

			if (d->allowsCustomBackground && (!d->customBackgroundPath.isEmpty() || d->customBackgroundColor.isValid())) {				
				if (!d->customBackgroundPath.isNull()) {
					if (d->customBackgroundPath.length() > 0) {
						switch (d->customBackgroundType) {
							case BackgroundNormal:
								bodyTag.append(QString::fromLatin1("background-image: url('%1'); background-repeat: no-repeat; background-attachment:fixed;").arg(d->customBackgroundPath));
							break;
							case BackgroundCenter:
								bodyTag.append(QString::fromLatin1("background-image: url('%1'); background-position: center; background-repeat: no-repeat; background-attachment:fixed;").arg(d->customBackgroundPath));
							break;
							case BackgroundTile:
								bodyTag.append(QString::fromLatin1("background-image: url('%1'); background-repeat: repeat;").arg(d->customBackgroundPath));
							break;
							case BackgroundTileCenter:
								bodyTag.append(QString::fromLatin1("background-image: url('%1'); background-repeat: repeat; background-position: center;").arg(d->customBackgroundPath));
							break;
							case BackgroundScale:
								bodyTag.append(QString::fromLatin1("background-image: url('%1'); -webkit-background-size: 100%% 100%%; background-size: 100%% 100%%; background-attachment: fixed;").arg(d->customBackgroundPath));
							break;
						}
					} else {
						bodyTag.append(QLatin1String("background-image: none; "));
					}
				}
				if (d->customBackgroundColor.isValid()) {
					qreal red, green, blue, alpha;
					d->customBackgroundColor.getRgbF(&red, &green, &blue, &alpha);
					bodyTag.append(QString::fromLatin1("background-color: rgba(%1, %2, %3, %4); ")
					               .arg(QString::number(int(red * 255.0)),
					                    QString::number(int(green * 255.0)),
					                    QString::number(int(blue * 255.0)),
					                    QString::number(alpha, 'f')));
				}
 			}
			
			//Replace the body background tag
			inString.replace(range, qstrlen(bodyBackground.latin1()), bodyTag);
 		}
 	}
	
	inString.replace(QLatin1String("%variant%"), activeVariantPath());

	return inString;
}

void WebKitMessageViewStyle::loadTemplates()
{
	Q_D(WebKitMessageViewStyle);
	//Load the style's templates
	d->headerHTML = loadResourceFile(QLatin1String("Header.html"));
	d->footerHTML = loadResourceFile(QLatin1String("Footer.html"));
	d->topicHTML = loadResourceFile(QLatin1String("Topic.html"));
	d->actionHTML = loadResourceFile(QLatin1String("Action.html"));
	d->baseHTML = loadResourceFile(QLatin1String("Template.html"));
	
	//Starting with version 1, styles can choose to not include template.html.  If the template is not included 
	//Adium's default will be used.  This is preferred since any future template updates will apply to the style
	if (d->baseHTML.isEmpty() && d->styleVersion >= 1) {
		QDir shareDir = ThemeManager::path(QLatin1String("data"), QLatin1String("webview"));
		Q_ASSERT(shareDir.exists(QLatin1String("Template.html")));
		QFile file(shareDir.filePath(QLatin1String("Template.html")));
		file.open(QFile::ReadOnly);
		d->baseHTML = QString::fromUtf8(file.readAll());
		d->usingCustomTemplateHTML = false;
	} else {
		d->usingCustomTemplateHTML = true;
		
		if (d->baseHTML.contains(QLatin1String("function imageCheck()"))) {
			/* This doesn't quite fix image swapping on styles with broken image swapping due to custom HTML templates,
			* but it improves it. For some reason, the result of using our normal template.html functions is that 
			* clicking works once, then the text doesn't allow a return click. This is an improvement compared
			* to fully broken behavior in which the return click shows a missing-image placeholder.
			*/
			QString imageSwapFixedBaseHTML = d->baseHTML;
			imageSwapFixedBaseHTML.replace(QLatin1String(
			"		function imageCheck() {\n"
			"			node = event.target;\n"
			"			if(node.tagName == 'IMG' && node.alt) {\n"
			"				a = document.createElement('a');\n"
			"				a.setAttribute('onclick', 'imageSwap(this)');\n"
			"				a.setAttribute('src', node.src);\n"
			"				text = document.createTextNode(node.alt);\n"
			"				a.appendChild(text);\n"
			"				node.parentNode.replaceChild(a, node);\n"
			"			}\n"
			"		}"
				), QLatin1String(
			"		function imageCheck() {\n"
			"			var node = event.target;\n"
			"			if(node.tagName.toLowerCase() == 'img' && !client.zoomImage(node) && node.alt) {\n"
			"				var a = document.createElement('a');\n"
			"				a.setAttribute('onclick', 'imageSwap(this)');\n"
			"				a.setAttribute('src', node.getAttribute('src'));\n"
			"				a.className = node.className;\n"
			"				var text = document.createTextNode(node.alt);\n"
			"				a.appendChild(text);\n"
			"				node.parentNode.replaceChild(a, node);\n"
			"			}\n"
			"		}"
				));
			imageSwapFixedBaseHTML.replace(QLatin1String(
			"		function imageSwap(node) {\n"
			"			img = document.createElement('img');\n"
			"			img.setAttribute('src', node.src);\n"
			"			img.setAttribute('alt', node.firstChild.nodeValue);\n"
			"			node.parentNode.replaceChild(img, node);\n"
			"			alignChat();\n"
			"		}"
				), QLatin1String(
			"		function imageSwap(node) {\n"
			"			var shouldScroll = nearBottom();\n"
			"			//Swap the image/text\n"
			"			var img = document.createElement('img');\n"
			"			img.setAttribute('src', node.getAttribute('src'));\n"
			"			img.setAttribute('alt', node.firstChild.nodeValue);\n"
			"			img.className = node.className;\n"
			"			node.parentNode.replaceChild(img, node);\n"
			"			\n"
			"			alignChat(shouldScroll);\n"
			"		}"
				));
			/* Now for ones which don't call alignChat() */
			imageSwapFixedBaseHTML.replace(QLatin1String(
			"		function imageSwap(node) {\n"
			"			img = document.createElement('img');\n"
			"			img.setAttribute('src', node.src);\n"
			"			img.setAttribute('alt', node.firstChild.nodeValue);\n"
			"			node.parentNode.replaceChild(img, node);\n"
			"		}"
				), QLatin1String(
			"		function imageSwap(node) {\n"
			"			var shouldScroll = nearBottom();\n"
			"			//Swap the image/text\n"
			"			var img = document.createElement('img');\n"
			"			img.setAttribute('src', node.getAttribute('src'));\n"
			"			img.setAttribute('alt', node.firstChild.nodeValue);\n"
			"			img.className = node.className;\n"
			"			node.parentNode.replaceChild(img, node);\n"
			"		}"
				));
			d->baseHTML = imageSwapFixedBaseHTML;
		}
		
	}
	
	//Content Templates
	d->contentHTML = loadResourceFile(QLatin1String("Content.html"));
	d->contentInHTML = loadResourceFile(QLatin1String("Content.html"), QLatin1String("Incoming"));
	d->nextContentInHTML = loadResourceFile(QLatin1String("NextContent.html"), QLatin1String("Incoming"));
	d->actionInHTML = loadResourceFile(QLatin1String("Action.html"), QLatin1String("Incoming"));
	d->contentOutHTML = loadResourceFile(QLatin1String("Content.html"), QLatin1String("Outgoing"));
	d->nextContentOutHTML = loadResourceFile(QLatin1String("NextContent.html"), QLatin1String("Outgoing"));
	d->actionOutHTML = loadResourceFile(QLatin1String("Action.html"), QLatin1String("Outgoing"));
	
	//Message history
	d->contextInHTML = loadResourceFile(QLatin1String("Context.html"), QLatin1String("Incoming"));
	d->nextContextInHTML = loadResourceFile(QLatin1String("NextContext.html"), QLatin1String("Incoming"));
	d->contextOutHTML = loadResourceFile(QLatin1String("Context.html"), QLatin1String("Outgoing"));
	d->nextContextOutHTML = loadResourceFile(QLatin1String("NextContext.html"), QLatin1String("Outgoing"));
	
	//Fall back to Resources/Content.html if Incoming isn't present
	if (d->contentInHTML.isEmpty())
		d->contentInHTML = d->contentHTML;
	
	//Fall back to Resources/Action.html if Incoming isn't present
	if (d->actionInHTML.isEmpty())
		d->actionInHTML = d->actionHTML;
	
	//Fall back to Content if NextContent doesn't need to use different HTML
	if (d->nextContentInHTML.isEmpty())
		d->nextContentInHTML = d->contentInHTML;
	
	//Fall back to Content if Context isn't present
	if (d->nextContextInHTML.isEmpty())
		d->nextContextInHTML = d->nextContentInHTML;
	if (d->contextInHTML.isEmpty())
		d->contextInHTML = d->contentInHTML;
	
	//Fall back to Content if Context isn't present
	if (d->nextContextOutHTML.isEmpty() && !d->nextContentOutHTML.isEmpty())
		d->nextContextOutHTML = d->nextContentOutHTML;
	if (d->contextOutHTML.isEmpty() && !d->contentOutHTML.isEmpty())
		d->contextOutHTML = d->contentOutHTML;
	
	//Fall back to Content if Context isn't present
	if (d->nextContextOutHTML.isEmpty())
		d->nextContextOutHTML = d->nextContextInHTML;
	if (d->contextOutHTML.isEmpty())
		d->contextOutHTML = d->contextInHTML;
	
	//Fall back to Incoming if Outgoing doesn't need to be different
	if (d->contentOutHTML.isEmpty())
		d->contentOutHTML = d->contentInHTML;
	if (d->nextContentOutHTML.isEmpty())
		d->nextContentOutHTML = d->nextContentInHTML;
	if (d->actionOutHTML.isEmpty())
		d->actionOutHTML = d->actionInHTML;
	
	//Status
	d->statusHTML = loadResourceFile(QLatin1String("Status.html"));
	
	//Fall back to Resources/Incoming/Content.html if Status isn't present
	if (d->statusHTML.isEmpty())
		d->statusHTML = d->contentInHTML;
	
	//TODO: make a generic Request message, rather than having this ft specific one
	d->fileTransferHTML = loadResourceFile(QLatin1String("FileTransferRequest.html"));
	if(d->fileTransferHTML.isEmpty()) {
		d->fileTransferHTML = d->contentInHTML;
		d->fileTransferHTML.replace(QLatin1String("%message%"),
		                            QLatin1String("<p><img src=\"%fileIconPath%\" style=\"width:32px; height:32px; vertical-align:middle;\"></img><input type=\"button\" onclick=\"%saveFileAsHandler%\" value=\"Download %fileName%\"></p>"));
	}
	d->fileTransferHTML.replace(QLatin1String("Download %fileName%"),
	                            QObject::tr("Download %fileName%"));
}

QString WebKitMessageViewStyle::templateForContent(const qutim_sdk_0_3::Message &message, bool contentIsSimilar)
{
	Q_D(WebKitMessageViewStyle);
	QString result;
	
	// Get the correct result for what we're inserting
	
	if (message.property("topic", false)) {
		result = d->topicHTML;
	// FIXME: Implement file transfer support
//	} else if (content.pro == IContent::FileTranfser) {
//		result = d->fileTransferHTML;
	} else if (!message.property("service", false)) {
		bool isAction = message.html().startsWith(QLatin1String("/me "), Qt::CaseInsensitive);
		if (isAction && hasAction()) {
			if (!message.isIncoming())
				result = d->actionOutHTML;
			else
				result = d->actionInHTML;
		} else if (message.property("history", false)) {
			if (!message.isIncoming())
				result = contentIsSimilar ? d->nextContextOutHTML : d->contextOutHTML;
			else
				result = contentIsSimilar ? d->nextContextInHTML : d->contextInHTML;
		} else {
			if (!message.isIncoming())
				result = contentIsSimilar ? d->nextContentOutHTML : d->contentOutHTML;
			else
				result = contentIsSimilar ? d->nextContentInHTML : d->contentInHTML;
		} 
	} else {
		result = d->statusHTML;
	} 
	
	if (!result.isEmpty())
		fillKeywords(result, message, contentIsSimilar);
	
	return result;
}

WebKitMessageViewStyle::UnitData WebKitMessageViewStyle::getSourceData(const qutim_sdk_0_3::Message &message)
{
	QObject *source = 0;
	UnitData result;
	result.id = message.property("senderId", QString());
	result.title = message.property("senderName", QString());
	if (!result.title.isEmpty()) {
		if (!result.id.isEmpty())
			source = message.chatUnit()->account()->getUnit(result.id, false);
		if (source)
			result.avatar = source->property("avatar").toString();
		return result;
	}
	if (!source && message.chatUnit()) {
		if (!message.isIncoming()) {
			const Conference *conf = qobject_cast<const Conference*>(message.chatUnit());
			if (conf && conf->me())
				source = conf->me();
			else
				source = message.chatUnit()->account();
		} else {
			source = message.chatUnit();
		}
	}
	if (!source)
		return result;
	result.avatar = source->property("avatar").toString();
	if (ChatUnit *unit = qobject_cast<ChatUnit*>(source)) {
		result.id = unit->id();
		result.title = unit->title();
	} else if (Account *account = qobject_cast<Account*>(source)) {
		result.id = account->id();
		result.title = account->name();
	}
	return result;
}

QString &WebKitMessageViewStyle::fillKeywords(QString &inString, const qutim_sdk_0_3::Message &message, bool contentIsSimilar)
{
	Q_D(WebKitMessageViewStyle);
	UnitData contentSource = getSourceData(message);
	UnitData theSource;
//	if (message.isIncoming() && message.chatUnit()->upperUnit())
//		theSource = getSourceData(message.chatUnit()->upperUnit());
//	else
		theSource = contentSource;
		
	QStringList displayClasses = message.property("displayClasses", QStringList());
	QString htmlEncodedMessage = message.html();
	
	// Actions support
	if (htmlEncodedMessage.startsWith(QLatin1String("/me "), Qt::CaseInsensitive)) {
		displayClasses << QLatin1String("action");
		htmlEncodedMessage.remove(0, 3);
		if (!hasAction())
			htmlEncodedMessage = QString::fromLatin1(ACTION_SPAN).arg(contentSource.title, htmlEncodedMessage);
	}

	//date
	QDateTime date = message.time();
	bool isService = message.property("service", false);
	bool isTopic = message.property("topic", false);
	bool isAutoreply = message.property("autoreply", false);
	
	//Replacements applicable to any AIContentObject
	inString.replace(QLatin1String("%time%"), convertTimeDate(d->timeStampFormatter, date));
	QString messageId = message.property("messageId").toString();
	if (messageId.isEmpty())
		messageId = QString::number(message.id());
	inString.replace(QLatin1String("%messageId%"), QLatin1String("message") + messageId);
	inString.replace(QLatin1String("%shortTime%"), date.toString(Qt::SystemLocaleShortDate));
	
	// FIXME: Implement
//	inString.replace(QLatin1String("%senderStatusIcon%"), QUrl)
//	if ([inString rangeOfString:@"%senderStatusIcon%"].location != NSNotFound) {
//		//Only cache the status icon to disk if the message style will actually use it
//		[inString replaceKeyword:@"%senderStatusIcon%"
//					  withString:[self statusIconPathForListObject:theSource]];
//	}

	
	inString.replace(QLatin1String("%userIcons%"), QLatin1String(d->showUserIcons ? "showIcons" : "hideIcons"));
	
	// Known classes:
	// "mention" == highlight
	// "history"
	// "consecutive"
	// "action" == /me
	// "firstFocus" == first received message after we switched to another tab
	// "focus" == we haven't seen this message
	if (message.property("focus", false))
		displayClasses << QLatin1String("focus");
	if (message.property("firstFocus", false))
		displayClasses << QLatin1String("firstFocus");
	if (isAutoreply)
		displayClasses << QLatin1String("autoreply");
	if (message.property("mention", false))
		displayClasses << QLatin1String("mention");
	if (!isTopic && isService) {
		displayClasses << QLatin1String("status");
		// Implement more logic way
		// May be status messages should provide information about previous and current statuses?
		displayClasses << message.property("statusType", QString());
	} else {
		if (message.property("history", false))
			displayClasses << QLatin1String("history");
		displayClasses << QLatin1String("message");
		displayClasses << QLatin1String(message.isIncoming() ? "incoming" : "outgoing");
	}
	inString.replace(QLatin1String("%messageClasses%"), QLatin1String(contentIsSimilar ? "consecutive " : "") + displayClasses.join(QLatin1String(" ")));
	
	inString.replace(QLatin1String("%senderColor%"), WebKitColorsAdditions::representedColorForObject(contentSource.id, validSenderColors()));
	
	inString.replace(QLatin1String("%messageDirection%"), QLatin1String(message.text().isRightToLeft() ? "rtl" : "ltr"));
	
	//Replaces %time{x}% with a timestamp formatted like x (using NSDateFormatter)
	const QStringMatcher matcher(QLatin1String("%time{"));
	const int matcherSize = matcher.pattern().size();
	const QStringMatcher endMatcher(QLatin1String("}%"));
	const int endMatcherSize = endMatcher.pattern().size();
	int range = 0;
	do {
		range = matcher.indexIn(inString, range);
		if (range != -1) {
			int endRange = endMatcher.indexIn(inString, range + matcherSize);
			if (endRange != -1) {
				QString timeFormat = inString.mid(range + matcherSize, endRange - range - matcherSize);
				QString time = convertTimeDate(timeFormat, date);
				inString.replace(range, endRange + endMatcherSize - range, time);
				range = range + time.size();
			}
		}
	} while (range != -1);
	
	QString userIconPath;
	if (d->showUserIcons)
		userIconPath = urlFromFilePath(theSource.avatar);
	if (userIconPath.isEmpty())
		userIconPath = QLatin1String(message.isIncoming() ? "Incoming/buddy_icon.png" : "Outgoing/buddy_icon.png");
	inString.replace(QLatin1String("%userIconPath%"), userIconPath);
	
	// Implement the way to get shortDescription and icon path for service icons
	QString service = message.chatUnit() ? message.chatUnit()->account()->protocol()->id() : QString();
	inString.replace(QLatin1String("%service%"), escapeString(service));
	inString.replace(QLatin1String("%serviceIconPath%"), QString() /*content.chat->account.protocol.iconPath*/);
	inString.replace(QLatin1String("%variant%"), activeVariantPath());

	bool replacedStatusPhrase = false;
	//message stuff
	if (isTopic || !isService) {
		//Use content.source directly rather than the potentially-metaContact theSource
		QString formattedUID = contentSource.id;
		QString displayName = contentSource.title;
		
		inString.replace(QLatin1String("%status%"), QString());
		inString.replace(QLatin1String("%senderScreenName%"), escapeString(formattedUID));
		// Should be used as %, @, + or something like irc's channel statuses
		inString.replace(QLatin1String("%senderPrefix%"), message.property("senderPrefix", QString()));
		QString senderDisplay = displayName;
		if (isAutoreply) {
			senderDisplay += " ";
			senderDisplay += QObject::tr("(Autoreply)");
		}
		inString.replace(QLatin1String("%sender%"), escapeString(senderDisplay));
		// Should be server-side display name if possible
		inString.replace(QLatin1String("%senderDisplayName%"), escapeString(displayName));

		// Add support for %textbackgroundcolor{alpha?}%
		// Background should be caught from content's html

		// FIXME: Add support
//		if ([content isKindOfClass:[ESFileTransfer class]]) { //file transfers are an AIContentMessage subclass
		
//			ESFileTransfer *transfer = (ESFileTransfer *)content;
//			NSString *fileName = [[transfer remoteFilename] stringByEscapingForXMLWithEntities:nil];
//			NSString *fileTransferID = [[transfer uniqueID] stringByEscapingForXMLWithEntities:nil];

//			range = [inString rangeOfString:@"%fileIconPath%"];
//			if (range.location != NSNotFound) {
//				NSString *iconPath = [self iconPathForFileTransfer:transfer];
//				NSImage *icon = [transfer iconImage];
//				do{
//					[[icon TIFFRepresentation] writeToFile:iconPath atomically:YES];
//					[inString safeReplaceCharactersInRange:range withString:iconPath];
//					range = [inString rangeOfString:@"%fileIconPath%"];
//				} while (range.location != NSNotFound);
//			}

//			[inString replaceKeyword:@"%fileName%"
//						  withString:fileName];
			
//			[inString replaceKeyword:@"%saveFileHandler%"
//						  withString:[NSString stringWithFormat:@"client.handleFileTransfer('Save', '%@')", fileTransferID]];
			
//			[inString replaceKeyword:@"%saveFileAsHandler%"
//						  withString:[NSString stringWithFormat:@"client.handleFileTransfer('SaveAs', '%@')", fileTransferID]];
			
//			[inString replaceKeyword:@"%cancelRequestHandler%"
//						  withString:[NSString stringWithFormat:@"client.handleFileTransfer('Cancel', '%@')", fileTransferID]];
//		}

		//Message (must do last)
		inString.replace(QLatin1String("%message%"), htmlEncodedMessage);
		
		// Topic replacement (if applicable)
		if (isTopic && inString.contains(QLatin1String("%topic%"))) {
			inString.replace(QLatin1String("%topic%"),
			                 QString::fromLatin1(TOPIC_INDIVIDUAL_WRAPPER).arg(htmlEncodedMessage));
		}		
	} else {
		inString.replace(QLatin1String("%status%"), escapeString(message.property("status", QString())));
		inString.replace(QLatin1String("%statusSender%"), QString());
		inString.replace(QLatin1String("%senderScreenName%"), QString());
		inString.replace(QLatin1String("%senderPrefix%"), QString());
		inString.replace(QLatin1String("%sender%"), QString());
		QString statusPhrase = message.property("statusPhrase", QString());
		if (!statusPhrase.isEmpty() && inString.contains(QLatin1String("%statusPhrase%"))) {
			inString.replace(QLatin1String("%statusPhrase%"), escapeString(statusPhrase));
			replacedStatusPhrase = true;
		}
		
		//Message (must do last)
		inString.replace(QLatin1String("%message%"), replacedStatusPhrase ? QString() : htmlEncodedMessage);
	}

	return inString;
}

QString WebKitMessageViewStyle::pathForResource(const QString &name, const QString &directory)
{
	Q_D(WebKitMessageViewStyle);
	QDir dir = d->stylePath;
	if (!directory.isEmpty() && !dir.cd(directory)) {
		QString fixedDirectory = dir.entryList(QStringList() << directory).value(0);
		if (fixedDirectory.isEmpty() || !dir.cd(fixedDirectory))
			return QString();
	}
	if (dir.exists(name))
		return dir.filePath(name);
	// Some old styles don't know about case-sensitive file systems
	QString fixedName = dir.entryList(QStringList() << name).value(0);
	if (fixedName.isEmpty())
		return QString();
	return dir.filePath(fixedName);
}

QString WebKitMessageViewStyle::loadResourceFile(const QString &name, const QString &directory)
{
	QString fileName = pathForResource(name, directory);
	if (fileName.isEmpty())
		return QString();
	QFile file(fileName);
	file.open(QFile::ReadOnly);
	QByteArray content = file.readAll();
	return QString::fromUtf8(content.constData(), content.size());
}

QString WebKitMessageViewStyle::stringWithFormat(const QString &text, const QStringList &args)
{
	int current = 0;
	int totalLength = text.length();
	for (int i = 0; i < args.size(); ++i)
		totalLength += args[i].length();
	QString result;
	result.reserve(totalLength);
	const QChar *str = text.constData();
	for (int i = 0; i < text.length(); ++i) {
		if (str[i] == QLatin1Char('%')) {
			if (str[i + 1] == QLatin1Char('%'))
				result += str[i];
			else if (str[i + 1] == QLatin1Char('@'))
				result += args.value(current++);
			else
				result += QLatin1String("{Error}");
			++i;
		} else {
			result += str[i];
		}
	}
	return result;
}

void WebKitMessageViewStyle::releaseResources()
{
	Q_D(WebKitMessageViewStyle);
	
	d->customStyle.clear();
	d->headerHTML.clear();
	d->footerHTML.clear();
	d->baseHTML.clear();
	d->contentHTML.clear();
	d->contentInHTML.clear();
	d->nextContentInHTML.clear();
	d->contextInHTML.clear();
	d->nextContextInHTML.clear();
	d->contentOutHTML.clear();
	d->nextContentOutHTML.clear();
	d->contextOutHTML.clear();
	d->nextContextOutHTML.clear();
	d->statusHTML.clear();	
	d->fileTransferHTML.clear();
	d->topicHTML.clear();
	d->actionHTML.clear();
	d->actionInHTML.clear();
	d->actionOutHTML.clear();
		
	d->customBackgroundPath.clear();
	d->customBackgroundColor = QColor();
	d->checkedSenderColors = false;
	d->checkedVariants = false;
	
	d->userIconMask = QImage();
}
