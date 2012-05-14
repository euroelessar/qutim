qutim_add_artwork_dir(qutim)
if(WIN32 OR HAIKU)
	install(DIRECTORY qutim/ca-certs DESTINATION ${QUTIM_SHARE_DIR})
endif() 
