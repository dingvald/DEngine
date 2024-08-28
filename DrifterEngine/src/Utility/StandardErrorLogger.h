#pragma once

struct ErrorLogger
{};

extern ErrorLogger error_logger;

template <typename T>
ErrorLogger& operator<< (ErrorLogger& el, const T& val)
{
	std::cerr << "\033[31m" << val;
	return el;
}

ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ostream&)) {
	f(std::cout << "\033[31m");
	return el;
}

ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ios&)) {
	f(std::cout << "\033[31m");
	return el;
}

ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ios_base&)) {
	f(std::cout << "\033[31m");
	return el;
}