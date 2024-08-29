#pragma once

struct ErrorLogger
{};

extern ErrorLogger error_logger;

template <typename T>
ErrorLogger& operator<< (ErrorLogger& el, const T& val)
{
	std::cerr << "\033[31m" << val << "\033[0m";
	return el;
}

inline ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ostream&)) {
	f(std::cerr << "\033[31m");
	return el << "\033[0m";
}

inline ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ios&)) {
	f(std::cerr << "\033[31m");
	return el << "\033[0m";
}

inline ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ios_base&)) {
	f(std::cerr << "\033[31m");
	return el << "\033[0m";
}