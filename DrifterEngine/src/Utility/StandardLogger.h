#pragma once

struct ErrorLogger
{};
struct WarningLogger
{};

extern ErrorLogger error_logger;
extern WarningLogger warning_logger;
extern const char* RedText;
extern const char* YellowText;
extern const char* DefaultText;


template <typename T>
ErrorLogger& operator<< (ErrorLogger& el, const T& val)
{
	std::cerr << RedText << val << DefaultText;
	return el;
}

inline ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ostream&)) {
	f(std::cerr << RedText);
	return el << DefaultText;
}

inline ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ios&)) {
	f(std::cerr << RedText);
	return el << DefaultText;
}

inline ErrorLogger& operator<< (ErrorLogger& el, std::ostream& (*f)(std::ios_base&)) {
	f(std::cerr << RedText);
	return el << DefaultText;
}

template <typename T>
WarningLogger& operator<< (WarningLogger& el, const T& val)
{
	std::cerr << YellowText << val << DefaultText;
	return el;
}

inline WarningLogger& operator<< (WarningLogger& el, std::ostream& (*f)(std::ostream&)) {
	f(std::cerr << YellowText);
	return el << DefaultText;
}

inline WarningLogger& operator<< (WarningLogger& el, std::ostream& (*f)(std::ios&)) {
	f(std::cerr << YellowText);
	return el << DefaultText;
}

inline WarningLogger& operator<< (WarningLogger& el, std::ostream& (*f)(std::ios_base&)) {
	f(std::cerr << YellowText);
	return el << DefaultText;
}

#define LOG_ERROR(msg) error_logger << "Error: " << msg << std::endl;
#define LOG_WARNING(msg) warning_logger << "Warning: " << msg << std::endl;