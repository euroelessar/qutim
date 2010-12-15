#ifdef W7CLDLL
#	define EXPORT   extern "C" __declspec(dllexport)
#	define SegFault ((int(*)())0) 
#else
#	define EXPORT   extern "C"
#endif

#ifdef __MINGW32__
#	define MINGWALIGN __attribute__ ((aligned (4)))
#else
#	define MINGWALIGN
#endif