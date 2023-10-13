#ifndef COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP
#define COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP

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
#include <windows.h>

namespace com::github::coderodde::wtpdmt::util {

	using std::string;

	// Priority class names:
	static const string ABOVE_NORMAL_PRIORITY_CLASS_STR   = "ABOVE_NORMAL_PRIORITY_CLASS";   // 0x00008000
	static const string BELOW_NORMAL_PRIORITY_CLASS_STR   = "BELOW_NORMAL_PRIORITY_CLASS";   // 0x00004000
	static const string HIGH_PRIORITY_CLASS_STR           = "HIGH_PRIORITY_CLASS";           // 0x00000080
	static const string IDLE_PRIORITY_CLASS_STR           = "IDLE_PRIORITY_CLASS";           // 0x00000040
	static const string NORMAL_PRIORITY_CLASS_STR         = "NORMAL_PRIORITY_CLASS";         // 0x00000020
	static const string PROCESS_MODE_BACKGROUND_BEGIN_STR = "PROCESS_MODE_BACKGROUND_BEGIN"; // 0x00100000
	static const string PROCESS_MODE_BACKGROUND_END_STR   = "PROCESS_MODE_BACKGROUND_END";   // 0x00200000
	static const string REALTIME_PRIORITY_CLASS_STR       = "REALTIME_PRIORITY_CLASS";       // 0x00000100

	// Thread priority names:
	static string THREAD_MODE_BACKGROUND_BEGIN_STR  = "THREAD_MODE_BACKGROUND_BEGIN";  // 0x0010000
	static string THREAD_MODE_BACKGROUND_END_STR    = "THREAD_MODE_BACKGROUND_END";    // 0x00020000
	static string THREAD_PRIORITY_ABOVE_NORMAL_STR  = "THREAD_PRIORITY_ABOVE_NORMAL";  // 1
	static string THREAD_PRIORITY_BELOW_NORMAL_STR  = "THREAD_PRIORITY_BELOW_NORMAL";  // -1
	static string THREAD_PRIORITY_HIGHEST_STR       = "THREAD_PRIORITY_HIGHEST";       // 2
	static string THREAD_PRIORITY_IDLE_STR          = "THREAD_PRIORITY_IDLE";          // -15
	static string THREAD_PRIORITY_LOWEST_STR        = "THREAD_PRIORITY_LOWEST";        // -2
	static string THREAD_PRIORITY_NORMAL_STR        = "THREAD_PRIORITY_NORMAL";        // 0
	static string THREAD_PRIORITY_TIME_CRITICAL_STR = "THREAD_PRIORITY_TIME_CRITICAL"; // 15

	// Program flags:
	static string FLAG_LONG_HELP                 = "--help";
	static string FLAG_LONG_NUMBER_OF_ITERATIONS = "--iterations";
	static string FLAG_LONG_PRIORITY_CLASS       = "--priority-class";
	static string FLAG_LONG_THREAD_PRIORITY      = "--thread-priority";

	static string FLAG_SHORT_HELP                 = "-h";
	static string FLAG_SHORT_NUMBER_OF_ITERATIONS = "-i";
	static string FLAG_SHORT_PRIORITY_CLASS       = "-p";
	static string FLAG_SHORT_THREAD_PRIORITY      = "-t";

	static const size_t DEFAULT_M_ITERATIONS      = 10 * 1000 * 1000;
	static const DWORD  DEFAULT_M_PRIORITY_CLASS  = NORMAL_PRIORITY_CLASS;
	static const int    DEFAULT_M_THREAD_PRIORITY = THREAD_PRIORITY_NORMAL;

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
		CommandLineParser(int argc, char* argv[]) :
			m_iteration_flag_present{ false },
			m_priority_class_flag_present{ false },
			m_thread_priority_flag_present{ false },
			m_help_flag_present{ false },
			m_argv{ argv },
			m_argc{ argc },
			m_argument_index{ 1 },
			m_iterations{ DEFAULT_M_ITERATIONS },
			m_priority_class{ DEFAULT_M_PRIORITY_CLASS},
			m_thread_priority{ DEFAULT_M_THREAD_PRIORITY }
		{
			loadDispatchMap();
			loadPriorityClassNameMap();
			loadThreadPriorityNameMap();
			loadPriorityClassNumberSet();
			loadThreadPriorityNumberSet();
			loadFlagSet();
			parseCommandLine();
		}

		size_t getNumberOfIterations() {
			return m_iterations;
		}

		DWORD getPriorityClass() {
			return m_priority_class;
		}

		int getThreadPriority() {
			return m_thread_priority;
		}

		bool helpRequested() {
			return m_help_flag_present;
		}

		void printHelp() {
			size_t maximum_class_name_length           = getMapStringKeyMaximumLength(m_priority_class_name_map);
			size_t maximum_thread_priority_name_length = getMapStringKeyMaximumLength(m_thread_priority_name_map);

			string programFileName = std::filesystem::path(m_argv[0]).filename().string();

			std::cout << programFileName
				<< " "
				<< "[" << FLAG_SHORT_PRIORITY_CLASS       << "|" << FLAG_LONG_PRIORITY_CLASS       << " <CLASS>] "
				<< "[" << FLAG_SHORT_THREAD_PRIORITY      << "|" << FLAG_LONG_THREAD_PRIORITY      << " <THREAD>] "
				<< "[" << FLAG_SHORT_NUMBER_OF_ITERATIONS << "|" << FLAG_LONG_NUMBER_OF_ITERATIONS << " <ITERATIONS>] "
				<< "[" << FLAG_SHORT_HELP                 << "|" << FLAG_LONG_HELP                 << "]\n"
				<< "where:\n\n"
				<< "  <CLASS> is one of: \n";

			std::cout << std::hex << std::setfill(' ');

			for (auto& i : m_priority_class_name_map) {
				std::cout 
				    << "    "
					<< std::setw(maximum_class_name_length)
					<< std::left
					<< std::setfill(' ')
					<< i.first
					<< " -- Sets the process class to 0x"
					<< std::hex
					<< std::right
					<< std::setfill('0')
					<< std::setw(8)
					<< i.second
					<< " = "
					<< std::dec
					<< i.second
					<< "\n";
			}

			std::cout << "\n  <THREAD> is one of:\n";

			for (auto& i : m_thread_priority_name_map) {
				std::cout << "    "
					<< std::setw(maximum_thread_priority_name_length)
					<< std::left
					<< std::setfill(' ')
					<< i.first
					<< " -- Sets the thread priority to 0x"
					<< std::hex
					<< std::right
					<< std::setfill('0')
					<< std::setw(8)
					<< i.second
					<< " = "
					<< std::dec
					<< i.second
					<< "\n";
			}

			std::cout << "\n";
		}

		string getPriorityClassName(DWORD priority_class) {
			for (const auto& p : m_priority_class_name_map) {
				if (p.second == priority_class) {
					return p.first;
				}
			}

			std::stringstream ss;
			
			ss << "Unknown priority class: 0x"
			   << std::setw(8)
			   << std::right
			   << std::setfill('0')
			   << std::hex 
			   << priority_class 
			   << " = " 
			   << std::dec 
			   << priority_class 
			   << ".";

			throw std::logic_error{ ss.str() };
		}

		string getThreadPriorityName(int thread_priority) {
			for (const auto& p : m_thread_priority_name_map) {
				if (p.second == thread_priority) {
					return p.first;
				}
			}

			std::stringstream ss;

			ss << "Unknown thread priority: 0x" 
			   << std::hex
			   << thread_priority 
			   << " = "
			   << std::dec
			   << thread_priority
			   << ".";

			throw std::logic_error{ ss.str() };
		}

		size_t getMaximumPriorityClassNameLength() {
			return getMapStringKeyMaximumLength(m_priority_class_name_map);
		}

		size_t getMaximumThreadPriorityNameLength() {
			return getMapStringKeyMaximumLength(m_thread_priority_name_map);
		}

	private:

		template<class Value>
		static size_t getMapStringKeyMaximumLength(std::map<string, Value>& map) {
			size_t maximum_string_length = 0;

			for (const auto& i : map) {
				maximum_string_length = max(maximum_string_length, i.first.length());
			}

			return maximum_string_length;
		}

		void loadDispatchMap() {
			std::function<void(CommandLineParser&)> function_flag_iterations = &CommandLineParser::processIterationFlags;

			m_flag_processor_map[FLAG_LONG_NUMBER_OF_ITERATIONS] = function_flag_iterations;
			m_flag_processor_map[FLAG_SHORT_NUMBER_OF_ITERATIONS] = function_flag_iterations;

			std::function<void(CommandLineParser&)> function_flag_priority_class = &CommandLineParser::processPriorityClassFlags;

			m_flag_processor_map[FLAG_LONG_PRIORITY_CLASS] = function_flag_priority_class;
			m_flag_processor_map[FLAG_SHORT_PRIORITY_CLASS] = function_flag_priority_class;

			std::function<void(CommandLineParser&)> function_flag_thread_priority = &CommandLineParser::processThreadPriorityFlags;

			m_flag_processor_map[FLAG_LONG_THREAD_PRIORITY] = function_flag_thread_priority;
			m_flag_processor_map[FLAG_SHORT_THREAD_PRIORITY] = function_flag_thread_priority;

			std::function<void(CommandLineParser&)> function_flag_help = &CommandLineParser::processHelpFlags;

			m_flag_processor_map[FLAG_LONG_HELP] = function_flag_help;
			m_flag_processor_map[FLAG_SHORT_HELP] = function_flag_help;
		}

		void loadPriorityClassNameMap() {
			m_priority_class_name_map[ABOVE_NORMAL_PRIORITY_CLASS_STR]   = ABOVE_NORMAL_PRIORITY_CLASS;
			m_priority_class_name_map[BELOW_NORMAL_PRIORITY_CLASS_STR]   = BELOW_NORMAL_PRIORITY_CLASS;
			m_priority_class_name_map[HIGH_PRIORITY_CLASS_STR]           = HIGH_PRIORITY_CLASS;
			m_priority_class_name_map[IDLE_PRIORITY_CLASS_STR]           = IDLE_PRIORITY_CLASS;
			m_priority_class_name_map[NORMAL_PRIORITY_CLASS_STR]         = NORMAL_PRIORITY_CLASS;
			m_priority_class_name_map[PROCESS_MODE_BACKGROUND_BEGIN_STR] = PROCESS_MODE_BACKGROUND_BEGIN;
			m_priority_class_name_map[PROCESS_MODE_BACKGROUND_END_STR]   = PROCESS_MODE_BACKGROUND_END;
			m_priority_class_name_map[REALTIME_PRIORITY_CLASS_STR]       = REALTIME_PRIORITY_CLASS;
		}

		void loadThreadPriorityNameMap() {
			m_thread_priority_name_map[THREAD_MODE_BACKGROUND_BEGIN_STR] = THREAD_MODE_BACKGROUND_BEGIN;
			m_thread_priority_name_map[THREAD_MODE_BACKGROUND_END_STR] = THREAD_MODE_BACKGROUND_END;
			m_thread_priority_name_map[THREAD_PRIORITY_ABOVE_NORMAL_STR] = THREAD_PRIORITY_ABOVE_NORMAL;
			m_thread_priority_name_map[THREAD_PRIORITY_BELOW_NORMAL_STR] = THREAD_PRIORITY_BELOW_NORMAL;
			m_thread_priority_name_map[THREAD_PRIORITY_HIGHEST_STR] = THREAD_PRIORITY_HIGHEST;
			m_thread_priority_name_map[THREAD_PRIORITY_IDLE_STR] = THREAD_PRIORITY_IDLE;
			m_thread_priority_name_map[THREAD_PRIORITY_LOWEST_STR] = THREAD_PRIORITY_LOWEST;
			m_thread_priority_name_map[THREAD_PRIORITY_NORMAL_STR] = THREAD_PRIORITY_NORMAL;
			m_thread_priority_name_map[THREAD_PRIORITY_TIME_CRITICAL_STR] = THREAD_PRIORITY_TIME_CRITICAL;
		}

		void loadPriorityClassNumberSet() {
			for (const auto& p : m_priority_class_name_map) {
				m_priority_class_number_set.emplace(p.second);
			}
		}

		void loadThreadPriorityNumberSet() {
			for (const auto& p : m_thread_priority_name_map) {
				m_thread_priority_number_set.emplace(p.second);
			}
		}

		void parseCommandLine() {
			while (m_argument_index < m_argc) {
				processFlagPair();

				if (m_help_flag_present) {
					return;
				}
			}
		}

		void loadFlagSet() {
			m_flag_set.emplace(FLAG_LONG_HELP);
			m_flag_set.emplace(FLAG_LONG_PRIORITY_CLASS);
			m_flag_set.emplace(FLAG_LONG_THREAD_PRIORITY);
			m_flag_set.emplace(FLAG_LONG_NUMBER_OF_ITERATIONS);

			m_flag_set.emplace(FLAG_SHORT_HELP);
			m_flag_set.emplace(FLAG_SHORT_PRIORITY_CLASS);
			m_flag_set.emplace(FLAG_SHORT_THREAD_PRIORITY);
			m_flag_set.emplace(FLAG_SHORT_NUMBER_OF_ITERATIONS);
		}

		void checkFlagIsValid(string& flag) {
			if (!m_flag_set.contains(flag)) {
				std::stringstream ss;
				ss << "Unknown flag: " << m_argv[m_argument_index] << ".";
				throw std::logic_error{ ss.str() };
			}
		}

		void checkMoreParametersAvailable() {
			if (m_argument_index == m_argc) {
				// Once here, the last flag has no value:
				std::stringstream ss;
				ss << "No value for the flag '" << m_argv[m_argc - 1] << ".";
				throw std::logic_error{ ss.str() };
			}
		}

		void processFlagPair() {
			string flag = m_argv[m_argument_index];

			if (flag == FLAG_LONG_HELP || flag == FLAG_SHORT_HELP) {
				m_help_flag_present = true;
				return;
			}

			// First check that the currently indexed flag is correct:
			checkFlagIsValid(flag);

			// Check that there is more parameters in the command line:
			m_argument_index++;
			checkMoreParametersAvailable();

			// Process the flag:
			(m_flag_processor_map[flag])(*this);
		}

		void processHelpFlags() {
			m_help_flag_present = true;
		}

		void processIterationFlags() {
			if (m_iteration_flag_present) {
				std::stringstream ss;
				ss << "The flag "
					<< FLAG_SHORT_NUMBER_OF_ITERATIONS
					<< " or "
					<< FLAG_LONG_NUMBER_OF_ITERATIONS
					<< " given more than once.";

				throw std::logic_error{ ss.str()};
			}

			m_iteration_flag_present = true;

			std::stringstream ss;
			ss << m_argv[m_argument_index++];
			ss >> m_iterations;
		}

		void processPriorityClassFlags() {
			if (m_priority_class_flag_present) {
				std::stringstream ss;
				ss << "The flag "
					<< FLAG_SHORT_PRIORITY_CLASS
					<< " or "
					<< FLAG_LONG_PRIORITY_CLASS
					<< " given more than once.";

				throw std::logic_error{ ss.str() };
			}

			auto pair = m_priority_class_name_map.find(m_argv[m_argument_index]);

			if (pair == m_priority_class_name_map.cend()) {

				string value = m_argv[m_argument_index];

				if (value.length() >= 3 && (value.substr(0, 2) == "0x" || value.substr(0, 2) == "0X")) {
					// Try parse as hexadecimal:
					std::istringstream iss(value);
					iss >> std::hex >> m_priority_class;

					if (iss.fail() || iss.bad()) {
						std::stringstream ss;
						ss << "Could not parse '" << value << "' as a valid hexadecimal value.";
						throw std::logic_error{ ss.str() };
					}
				}
				else {
					// Try parse as decimal:
					std::istringstream iss(value);
					iss >> std::dec >> m_priority_class;

					if (iss.fail() || iss.bad()) {
						std::stringstream ss;
						ss << "Could not parse '" << value << "' as a valid decimal value.";
						throw std::logic_error{ ss.str() };
					}
				}				
			} else {
				m_priority_class = pair->second;
			}

			m_priority_class_flag_present = true;
			m_argument_index++;
		}

		void processThreadPriorityFlags() {
			if (m_thread_priority_flag_present) {
				std::stringstream ss;
				ss << "The flag "
					<< FLAG_SHORT_THREAD_PRIORITY
					<< " or "
					<< FLAG_LONG_THREAD_PRIORITY
					<< " given more than once.";

				throw std::logic_error{ ss.str() };
			}

			auto pair = m_thread_priority_name_map.find(m_argv[m_argument_index]);

			if (pair == m_thread_priority_name_map.cend()) {
				string value = m_argv[m_argument_index];

				if (value.length() >= 3 && (value.substr(0, 2) == "0x" || value.substr(0, 2) == "0X")) {
					// Try parse as hexadecimal:
					std::istringstream iss(value);
					iss >> std::hex >> m_thread_priority;

					if (iss.fail() || iss.bad()) {
						std::stringstream ss;
						ss << "Could not parse '" << value << "' as a valid hexadecimal value.";
						throw std::logic_error{ ss.str() };
					}
				} else {
					// Try parse as decimal:
					std::istringstream iss(value);
					iss >> std::dec >> m_thread_priority;

					if (iss.fail() || iss.bad()) {
						std::stringstream ss;
						ss << "Could not parse '" << value << "' as a valid decimal value.";
						throw std::logic_error{ ss.str() };
					}
				}
			} else {
				m_thread_priority = pair->second;
			}

			m_thread_priority_flag_present = true;
			m_argument_index++;
		}
	};
} // End of namespace com::github::coderodde::wtpdmt::util
#endif // COM_GITHUB_CODERODDE_WTPDMT_UTIL_COMMAND_LINE_PARSER_HPP