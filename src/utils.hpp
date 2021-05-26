namespace Utilities {
typedef enum { MSG_STATUS_SUCCESS, MSG_STATUS_WARNING, MSG_STATUS_ERROR } Status;

int print(Utilities::Status status, const char* fmt, ...);
}
