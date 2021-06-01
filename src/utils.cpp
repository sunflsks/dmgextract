#include "utils.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif // WIN32

static const char* win32_forbidden_filename_characters = "<>:\"\\|?*";
static int get_terminal_width();

int Utilities::print(Utilities::Status status, const char* fmt, ...) {
    FILE* output_stream = nullptr;
    const char* prefix = nullptr;
    const char* reset = "\033[0m";

#ifdef WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console, &info);

    switch (status) {
        case MSG_STATUS_SUCCESS: {
            SetConsoleTextAttribute(console, FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            output_stream = stdout;
            break;
        }

        case MSG_STATUS_WARNING: {
            SetConsoleTextAttribute(console,
                                    FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_RED);
            output_stream = stderr;
            break;
        }

        case MSG_STATUS_ERROR: {
            SetConsoleTextAttribute(console, FOREGROUND_INTENSITY | FOREGROUND_RED);
            output_stream = stderr;
            break;
        }
    }
#else
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
#endif // WIN32

    va_list args;
    va_start(args, fmt);

    int ret = vfprintf(output_stream, fmt, args);
    va_end(args);
#ifdef WIN32
    SetConsoleTextAttribute(console, info.wAttributes);
#else
    fputs(reset, output_stream);
#endif // WIN32
    return ret;
}

void Utilities::print_progress(uint64_t current_progress, uint64_t total_progress, bool final) {
    int width =
      get_terminal_width() - 3; // subtract 2 for the opening and closing brackets on the bar, and
                                // one more because the windows console automatically goes to a new
                                // line when something is printed at the end of the console

    int how_much_to_print = (((float)current_progress / total_progress) * width);
    how_much_to_print = how_much_to_print < width ? how_much_to_print : width;

    putc('[', stdout);

    int curpos;
    for (curpos = 0; curpos < how_much_to_print; curpos++) {
        putc('=', stdout);
    }

    for (int i = curpos; i < width; i++) {
        putc(' ', stdout);
    }

    if (final) {
        fputs("]\n", stdout);
    } else {
        fputs("]\r", stdout);
        fflush(stdout);
    }
}

static int get_terminal_width() {
    int ret;
#ifdef WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    ret = info.dwSize.X;
#else
    struct winsize ws;
    ioctl(1, TIOCGWINSZ, &ws);
    ret = ws.ws_col;
#endif
    return ret;
}

void Utilities::win32_get_sanitized_filename(std::string& input, char replace) {
    char* buf = input.data();

    for (uint64_t i = 0; i < input.length(); i++) {
        if (strchr(win32_forbidden_filename_characters, buf[i])) {
            if (dmgextract_verbose) {
                print(MSG_STATUS_ERROR, "Replacing forbidden character in %s\n", input.c_str());
            }
            buf[i] = replace;
        }
    }
}
