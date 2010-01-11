#cmakedefine HAVE_ZLIB
#cmakedefine HAVE_WINTLS
#cmakedefine HAVE_GNUTLS
#cmakedefine HAVE_OPENSSL
#cmakedefine HAVE_LIBIDN
#cmakedefine HAVE_GETADDRINFO
#cmakedefine HAVE_SETSOCKOPT
#cmakedefine HAVE_WINDNS_H

#ifdef _MSC_VER
#pragma comment( lib, "Dnsapi.lib" )
#pragma comment( lib, "Secur32.lib" )
#pragma comment( lib, "Crypt32.lib" )
#pragma comment( lib, "ws2_32.lib" )
#pragma warning( disable : 4355 )
#pragma warning( disable : 4996 )
#endif
