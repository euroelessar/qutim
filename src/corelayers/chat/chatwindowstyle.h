#ifndef CHATWINDOWSTYLE_H_
#define CHATWINDOWSTYLE_H_

#include <QHash>
#include <QDateTime>

class QWebPage;

namespace AdiumMessageStyle {

class WeekDate
{
public:
	inline WeekDate(const QDate &date) { setDate(date); }
	inline WeekDate(int y, int m, int d) { setDate(QDate(y, m, d)); }
	inline int year() const { return m_year; }
	inline int week() const { return m_week; }
	inline int day() const { return m_day; }
	void setDate(const QDate &date);
private:
	int m_year;
	int m_week;
	int m_day;
};

class ChatWindowStyle
{
public:
	/**
	 * StyleVariants is a typedef to a QMap
	 * key = Variant Name
	 * value = Path to variant CSS file.
	 * Path is relative to Ressources directory.
	 */
	typedef QHash<QString,QString> StyleVariants;

	/**
	 * This enum specifies the mode of the constructor
	 * - StyleBuildFast : Build the style the fatest possible
	 * - StyleBuildNormal : List all variants of this style. Require a async dir list.
	 */
	enum StyleBuildMode { StyleBuildFast, StyleBuildNormal};

	/**
	 * @brief Build a single chat window style.
	 *
	 */
	explicit ChatWindowStyle(const QString &styleName, StyleBuildMode styleBuildMode = StyleBuildNormal);
	ChatWindowStyle(const QString &styleName, const QString &variantPath, StyleBuildMode styleBuildMode = StyleBuildFast);
	~ChatWindowStyle();

	/**
	 * Get the list of all variants for this theme.
	 * If the variant aren't listed, it call the lister
	 * before returning the list of the Variants.
	 * If the variant are listed, it just return the cached
	 * variant list.
	 * @return the StyleVariants QMap.
	 */
	StyleVariants getVariants();

	/**
	 * Get the style path.
	 * The style path points to the directory where the style is located.
	 * ex: ~/.kde/share/apps/kopete/styles/StyleName/
	 *
	 * @return the style path based.
	 */
	QString getStyleName() const;

	/**
	 * Get the style ressource directory.
	 * Ressources directory is the base where all CSS, HTML and images are located.
	 *
	 * Adium(and now Kopete too) style directories are disposed like this:
	 * StyleName/
	 *          Contents/
	 *            Resources/
	 *
	 * @return the path to the the ressource directory.
	 */
	QString getStyleBaseHref() const;
	QString getTemplateHtml() const;
	QString getHeaderHtml() const;
	QString getFooterHtml() const;
	QString getIncomingHtml() const;
	QString getNextIncomingHtml() const;
	QString getOutgoingHtml() const;
	QString getNextOutgoingHtml() const;
	QString getIncomingHistoryHtml() const;
	QString getNextIncomingHistoryHtml() const;
	QString getOutgoingHistoryHtml() const;
	QString getNextOutgoingHistoryHtml() const;
	QString getIncomingActionHtml() const;
	QString getOutgoingActionHtml() const;
	QString getStatusHtml() const;
	QString getMainCSS() const;

	/**
	 * Reload style from disk.
	 */
	void reload();
	/**
	 * It should be equal to NSDateFormatter of MacOS X
	 */
	static QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);

	void preparePage(QWebPage *page) const;
private:
	/**
	 * Read style HTML files from disk
	 */
	void readStyleFiles();

	/**
	 * Init this class
	 */
	void init(const QString &styleName, StyleBuildMode styleBuildMode);

	/**
	 * List available variants for the current style.
	 */
	void listVariants();

private:
	class Private;
	Private *d;
};

}

#endif /*CHATWINDOWSTYLE_H_*/
