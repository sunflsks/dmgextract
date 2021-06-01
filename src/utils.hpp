#include <cstdint>
#include <string>

extern bool dmgextract_verbose;

namespace Utilities {
typedef enum { MSG_STATUS_SUCCESS, MSG_STATUS_WARNING, MSG_STATUS_ERROR } Status;

int print(Utilities::Status status, const char* fmt, ...);
void print_progress(uint64_t current, uint64_t total, bool final);
void win32_get_sanitized_filename(std::string& input, char replace);
}
