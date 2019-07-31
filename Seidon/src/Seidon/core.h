#pragma once

#ifdef SD_WINDOWS
	#ifdef SD_DLL
		#define SEIDON_API __declspec(dllexport)
	#else
		#define SEIDON_API __declspec(dllimport)
	#endif
#endif

