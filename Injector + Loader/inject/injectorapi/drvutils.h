#pragma once
#include "api.h"
#include "../driver/driver.h"

void start_driver()
{
	driver().handle_driver();

	if (!driver().is_loaded())


		// DEBUG_PRINT
	{
#ifdef DEBUG_PRINT
		cout << xor_a("driver initialize...") << endl;
#endif 		
		mmap_driver();
	}

	driver().handle_driver();

	if (!driver().is_loaded())
	{
		MessageBoxA(0, "Driver Failed to Load or Map.", "", MB_ICONERROR);
		exit(-69);
	}
#ifdef DEBUG_PRINT
	driver().is_loaded() ? cout << xor_a("driver initialized!") << endl : cout << xor_a("driver initialize error =<") << endl;
	getchar();
#endif 		
}

