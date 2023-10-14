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

	//// Priority class names:
	//static const string ABOVE_NORMAL_PRIORITY_CLASS_STR   = "ABOVE_NORMAL_PRIORITY_CLASS";   // 0x00008000
	//static const string BELOW_NORMAL_PRIORITY_CLASS_STR   = "BELOW_NORMAL_PRIORITY_CLASS";   // 0x00004000
	//static const string HIGH_PRIORITY_CLASS_STR           = "HIGH_PRIORITY_CLASS";           // 0x00000080
	//static const string IDLE_PRIORITY_CLASS_STR           = "IDLE_PRIORITY_CLASS";           // 0x00000040
	//static const string NORMAL_PRIORITY_CLASS_STR         = "NORMAL_PRIORITY_CLASS";         // 0x00000020
	//static const string PROCESS_MODE_BACKGROUND_BEGIN_STR = "PROCESS_MODE_BACKGROUND_BEGIN"; // 0x00100000
	//static const string PROCESS_MODE_BACKGROUND_END_STR   = "PROCESS_MODE_BACKGROUND_END";   // 0x00200000
	//static const string REALTIME_PRIORITY_CLASS_STR       = "REALTIME_PRIORITY_CLASS";       // 0x00000100

	//// Thread priority names:
	//static string THREAD_MODE_BACKGROUND_BEGIN_STR  = "THREAD_MODE_BACKGROUND_BEGIN";  // 0x0010000
	//static string THREAD_MODE_BACKGROUND_END_STR    = "THREAD_MODE_BACKGROUND_END";    // 0x00020000
	//static string THREAD_PRIORITY_ABOVE_NORMAL_STR  = "THREAD_PRIORITY_ABOVE_NORMAL";  // 1
	//static string THREAD_PRIORITY_BELOW_NORMAL_STR  = "THREAD_PRIORITY_BELOW_NORMAL";  // -1
	//static string THREAD_PRIORITY_HIGHEST_STR       = "THREAD_PRIORITY_HIGHEST";       // 2
	//static string THREAD_PRIORITY_IDLE_STR          = "THREAD_PRIORITY_IDLE";          // -15
	//static string THREAD_PRIORITY_LOWEST_STR        = "THREAD_PRIORITY_LOWEST";        // -2
	//static string THREAD_PRIORITY_NORMAL_STR        = "THREAD_PRIORITY_NORMAL";        // 0
	//static string THREAD_PRIORITY_TIME_CRITICAL_STR = "THREAD_PRIORITY_TIME_CRITICAL"; // 15

	//// Program flags:
	//static string FLAG_LONG_HELP                 = "--help";
	//static string FLAG_LONG_NUMBER_OF_ITERATIONS = "--iterations";
	//static string FLAG_LONG_PRIORITY_CLASS       = "--priority-class";
	//static string FLAG_LONG_THREAD_PRIORITY      = "--thread-priority";

	//static string FLAG_SHORT_HELP                 = "-h";
	//static string FLAG_SHORT_NUMBER_OF_ITERATIONS = "-i";
	//static string FLAG_SHORT_PRIORITY_CLASS       = "-p";
	//static string FLAG_SHORT_THREAD_PRIORITY      = "-t";

	//static const size_t DEFAULT_M_ITERATIONS      = 10 * 1000 * 1000;
	//static const DWORD  DEFAULT_M_PRIORITY_CLASS  = NORMAL_PRIORITY_CLASS;
	//static const int    DEFAULT_M_THREAD_PRIORITY = THREAD_PRIORITY_NORMAL;

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
	};
} // End of namespace com::github::coderodde::wtpdmt::util
#endif // COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP