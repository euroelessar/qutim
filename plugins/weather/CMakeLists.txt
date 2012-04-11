qutim_add_plugin( weather
	DISPLAY_NAME "Weather plugin"
	DESCRIPTION "Plugin shows a current weather in your city."
	QT_LIBRARIES NETWORK XML
)

qutim_add_artwork(weatherthemes)
