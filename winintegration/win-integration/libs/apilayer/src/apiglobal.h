#ifdef WT_APIL_LIB
#	define EXPORT   extern "C" __declspec(dllexport)
#	define SegFault ((int(*)())0) 
#else
#	define EXPORT   extern "C"
#endif
