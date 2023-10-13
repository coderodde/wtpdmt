#include "CommandLineParser.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <windows.h>

using std::string;

// BEGIN OF: https://stackoverflow.com/a/17387176/1049393
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = GetLastError();

	if (errorMessageID == 0) {
		return string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
								 FORMAT_MESSAGE_FROM_SYSTEM     | 
								 FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL,
								 errorMessageID, 
								 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
								 (LPSTR)& messageBuffer,
								 0, 
								 NULL);

	//Copy the error message into a std::string.
	string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}
// END OF: https://stackoverflow.com/a/17387176/1049393

int main(int argc, char* argv[]) try {
	com::github::coderodde::wtpdmt::util::CommandLineParser clp(argc, argv);

	if (clp.helpRequested()) {
		clp.printHelp();
		return EXIT_SUCCESS;
	}

	string priority_class_name  = clp.getPriorityClassName(clp.getPriorityClass());
	string thread_priority_name = clp.getThreadPriorityName(clp.getThreadPriority());

	size_t field_length = max(priority_class_name.length(), thread_priority_name.length());
	
	std::cout << "Number of iterations:           " 
		      << clp.getNumberOfIterations()
		      << "\n";

	std::cout << "Requested priority class:       " 
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

	std::cout << "Requested thread priority:      " 
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
		      << ")\n";

	HANDLE process_handle = GetCurrentProcess();
	HANDLE thread_handle  = GetCurrentThread();

	if (!SetPriorityClass(process_handle, clp.getPriorityClass())) {
		std::cerr << "WARNING: Could not set the process class. Windews returned: "
		          << GetLastErrorAsString() 
				  << ".\n";
	}

	if (!SetThreadPriority(thread_handle, clp.getThreadPriority())) {
		std::cerr << "WANRING: Could not set the thread priority. Windows returned: "
		          << GetLastErrorAsString()
				  << ".\n";
	}

	std::cout << "INFO: Effective process class is   " 
			  << clp.getPriorityClassName(GetPriorityClass(process_handle))
			  << ".\n";

	std::cout << "INFO: Effective thread priority is "
			  << clp.getThreadPriorityName(GetThreadPriority(thread_handle))
			  << ".\n";

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