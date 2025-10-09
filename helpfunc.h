#ifndef HELPFUNC_HEADER
#define HELPFUNC_HEADER

void PrintErrorInfo(const char* file, const char* func, int line, const char* message, ...);

void FprintfStack_t(FILE* fp, Stack_t elem, const char* afterword);

size_t SizeOfFile(FILE* fp);

#endif
