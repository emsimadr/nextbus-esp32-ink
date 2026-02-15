#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <stddef.h>

// Fetch the /v1/board response from the Bus Tracker API.
// Writes the response body into `buffer` (up to `bufLen` bytes).
// Returns the HTTP status code (200, 401, 404, 503, etc.)
// or -1 on network/connection error.
// See: .cursor/rules/project.mdc > API Contract
int fetchBoard(const char* baseUrl, const char* apiKey, char* buffer, size_t bufLen);

#endif // API_CLIENT_H
