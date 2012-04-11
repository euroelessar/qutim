if(BEOS)
	qutim_add_plugin(haikunotifications
		DISPLAY_NAME "Haiku notifications"
		DESCRIPTION "Notification system integrated into Haiku"
	)
else()
	message(STATUS "'Haiku Notifications' plugin will be skipped.")
endif()
