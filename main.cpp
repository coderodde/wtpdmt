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

int main(int argc, char* argv[]) try {
	com::github::coderodde::wtpdmt::util::CommandLineParser clp(argc, argv);

	string priority_class_name  = clp.getPriorityClassName(clp.getPriorityClass());
	string thread_priority_name = clp.getThreadPriorityName(clp.getThreadPriority());

	size_t field_length = max(priority_class_name.length(), thread_priority_name.length());

	std::cout << "Number of iterations: " 
		      << clp.getNumberOfIterations()
		      << "\n";

	std::cout << "Priority class:       " 
		      << std::setw(field_length)
		      << std::left 
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

	std::cout << "Thread priority:      " 
		      << std::setw(field_length) 
		      << std::left
		      << thread_priority_name
		      << " (0x"
		      << std::setw(8)
		      << std::right
		      << std::setfill('0')
		      << std::hex
		      << clp.getThreadPriority() 
		      << std::dec
		      << clp.getThreadPriority()
		      << ")\n";

	std::cout << "Help flag on:         " << std::boolalpha << clp.helpRequested() << "\n";

	clp.printHelp();

	ULONGLONG ta = GetTickCount64();
	ULONGLONG maximumDuration = 0;

	for (int i = 0; i < 10 * 1000; i++) {
		ULONGLONG tb = GetTickCount64();
		ULONGLONG duration = tb - ta;
		maximumDuration = max(maximumDuration, duration);
	}

	printf("%llu\n", maximumDuration);
	return 0;
} catch (std::logic_error& err) {
	std::cerr << "ERROR: " << err.what() << "\n";
}