#include <Callstack_Logger.h>
#include <iostream>

std::string Callstack_Logger::Indent;

Callstack_Logger::Callstack_Logger(const char* fileName, const char* funcName, int lineNumber) {
    file_name = fileName;
    func_name = funcName;
    std::cout << Indent  << func_name << " Start - (" << file_name << ":" << lineNumber<< ")" << std::endl;
    Indent.append("\t");
}

Callstack_Logger::~Callstack_Logger() {
    Indent.resize ( Indent.length() - 2);
    std::cout << Indent << func_name << " End - (" << file_name << ")" << std::endl;
}
