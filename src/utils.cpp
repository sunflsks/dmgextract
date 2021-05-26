#include "utils.hpp"
#include <cstdarg>
#include <cstdio>

int Utilities::print(Utilities::Status status, const char* fmt, ...) {
    FILE* output_stream = nullptr;
    const char* prefix = nullptr;
    const char* reset = "\033[0m";

    switch (status) {
        case MSG_STATUS_SUCCESS: {
            output_stream = stdout;
            prefix = "\033[1;32m";
            break;
        }

        case MSG_STATUS_WARNING: {
            output_stream = stderr;
            prefix = "\033[1;33m";
            break;
        }

        case MSG_STATUS_ERROR: {
            output_stream = stderr;
            prefix = "\033[1;31m";
            break;
        }

        default:
            output_stream = stdout;
            prefix = "";
            break;
    }

    fprintf(output_stream, prefix);

    va_list args;
    va_start(args, fmt);

    int ret = vfprintf(output_stream, fmt, args);
    va_end(args);
    fputs(reset, output_stream);
    return ret;
}
