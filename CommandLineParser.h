#ifndef COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP
#define COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP

#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <windows.h>

namespace com::github::coderodde::wtpdmt::util {

	using std::string;

	class CommandLineParser {
	private:

		bool m_iteration_flag_present;
		bool m_priority_class_flag_present;
		bool m_thread_priority_flag_present;
		bool m_help_flag_present;

		char** m_argv;
		int m_argc;
		int m_argument_index;

		size_t m_iterations;
		DWORD m_priority_class;
		int m_thread_priority;

		std::unordered_map<string, std::function<void(CommandLineParser&)>> m_flag_processor_map;
		std::map<string, DWORD>    m_priority_class_name_map;
		std::map<string, int>      m_thread_priority_name_map;
		std::unordered_set<DWORD>  m_priority_class_number_set;
		std::unordered_set<int>    m_thread_priority_number_set;
		std::unordered_set<string> m_flag_set;

	public:
	
		CommandLineParser(int argc, char* argv[]);

		size_t getNumberOfIterations();
		DWORD getPriorityClass();
		int getThreadPriority();
		bool helpRequested();
		void printHelp();
		string getPriorityClassName(DWORD priority_class);
		string getThreadPriorityName(int thread_priority);
		size_t getMaximumPriorityClassNameLength();
		size_t getMaximumThreadPriorityNameLength();

	private:
	
		void loadDispatchMap();
		void loadPriorityClassNameMap();
		void loadThreadPriorityNameMap();
		void loadPriorityClassNumberSet();
		void loadThreadPriorityNumberSet();
		void loadFlagSet();
		void parseCommandLine();
		void checkFlagIsValid(string& flag);
		void checkMoreParametersAvailable();
		void processFlagPair();
		void processHelpFlags();
		void processIterationFlags();
		void processPriorityClassFlags();
		void processThreadPriorityFlags();
		template<class T>
		void parseValue(string& value, T* target_ptr);
	};
} // End of namespace com::github::coderodde::wtpdmt::util
#endif // COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP