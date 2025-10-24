#ifndef SPU_HELP_FUNCS_HEADER
#define SPU_HELP_FUNCS_HEADER

#define PRINT_LOG_FILE_SPU(X, ...) fprintf(logfileProc, "%s:%d %s: " X,__FILE__, __LINE__, __func__, ##__VA_ARGS__);\
                                   fflush(logfileProc)

int SpuOpenLogFile();

void SpuCloseLogFile();

#endif
