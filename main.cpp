#include "CommandLineParser.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <windows.h>

using std::string;

int main(int argc, char* argv[]) try {
	com::github::coderodde::wtpdmt::util::CommandLineParser clp(argc, argv);

	if (clp.helpRequested()) {
		clp.printHelp();
		return EXIT_SUCCESS;
	}

	string priority_class_name  = clp.getPriorityClassName(clp.getPriorityClass());
	string thread_priority_name = clp.getThreadPriorityName(clp.getThreadPriority());

	size_t field_length = max(priority_class_name.length(),
							  thread_priority_name.length());
	
	std::cout << "Number of iterations:      " 
		      << clp.getNumberOfIterations()
		      << "\n";

	std::cout << "Requested priority class:  " 
		      << std::setw(field_length)
		      << std::left 
			  << std::setfill(' ')
		      << priority_class_name
		      << " (0x" 
		      << std::setw(8)
		      << std::right
		      << std::setfill('0')
		      << std::hex
		      << clp.getPriorityClass()
		      << " = "
		      << std::dec
		      << clp.getPriorityClass()
		      << ")\n";

	std::cout << "Requested thread priority: " 
		      << std::setw(field_length) 
		      << std::left
			  << std::setfill(' ')
		      << thread_priority_name
		      << " (0x"
		      << std::setw(8)
		      << std::right
		      << std::setfill('0')
		      << std::hex
		      << clp.getThreadPriority() 
			  << " = "
		      << std::dec
		      << clp.getThreadPriority()
		      << ")\n\n";

	HANDLE process_handle = GetCurrentProcess();
	HANDLE thread_handle  = GetCurrentThread();

	SetPriorityClass(process_handle, clp.getPriorityClass());
	SetThreadPriority(thread_handle, clp.getThreadPriority());

	std::cout << "INFO: Effective process class is   " 
			  << clp.getPriorityClassName(GetPriorityClass(process_handle))
			  << ".\n";

	std::cout << "INFO: Effective thread priority is "
			  << clp.getThreadPriorityName(GetThreadPriority(thread_handle))
			  << ".\n\n";

	ULONGLONG ta = GetTickCount64();
	ULONGLONG maximumDuration = 0;

	for (int i = 0; i < clp.getNumberOfIterations(); i++) {
		ULONGLONG tb = GetTickCount64();
		ULONGLONG duration = tb - ta;
		maximumDuration = max(maximumDuration, duration);
	}

	std::cout << maximumDuration << "\n";
	return EXIT_SUCCESS;
} catch (std::logic_error& err) {
	std::cerr << "ERROR: " << err.what() << "\n";
	return EXIT_FAILURE;
}