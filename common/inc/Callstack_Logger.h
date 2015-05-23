#ifndef CALLSTACK_LOGGER
#define CALLSTACK_LOGGER

#include <string>

class Callstack_Logger
{
    public:
        Callstack_Logger(const char* _file_name, const char* _func_name, int _line_number);
        ~Callstack_Logger();
                    
    private:
        const char* file_name;
        const char* func_name;

        static std::string Indent;
};

#endif
