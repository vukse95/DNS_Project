#ifndef D_SERVER
#define D_SERVER

#define D_MAX_NUMBER_OF_AUTOMATES       10

#define D_VALID_NUMBER_OF_ARGUMENTS     2

#define D_ERROR                         -1
#define D_OK                            0

void initServer(char * pcIpParam);

void mainProgram(int argc, char* argv[]);


#endif