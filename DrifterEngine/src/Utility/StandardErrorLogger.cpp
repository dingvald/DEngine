#include "pch.h"
#include "StandardErrorLogger.h"

ErrorLogger error_logger;
WarningLogger warning_logger;

const char* RedText = "\033[31m";
const char* YellowText = "\033[33m";
const char* DefaultText = "\033[0m";