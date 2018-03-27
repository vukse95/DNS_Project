// LogFile.h: interface for the LogFile class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _LOG_FILE_
#define _LOG_FILE_

#include <fstream>
#include <stdio.h>
#include <time.h>
#include "logAutomate.h"
#ifdef __LINUX__
#include <stdlib.h>
#endif

#define MAX_NUM_OF_MESSAGES        100
#define MAX_NUM_OF_TIMERS           20
#define MAX_NUM_OF_AUTOMATE_TYPES   50
#define MAX_NUM_OF_MSG_CODING_TYPES  5
#define MAX_NAME_LENGTH             51

struct StructMsgData {
  char   msgName[MAX_NUM_OF_MESSAGES][MAX_NAME_LENGTH];
  uint16 msgCode[MAX_NUM_OF_MESSAGES];
  uint16 NumOfMsgs;
};


struct StructAutomateData{
  char   automateName[MAX_NAME_LENGTH];
  char   timerName[MAX_NUM_OF_TIMERS][MAX_NAME_LENGTH];
};

class LogFile : public LogInterface {
  private:
    char FileName[60];
    char IniFileName[60];
    struct StructAutomateData AutomateData[MAX_NUM_OF_AUTOMATE_TYPES];
    struct StructMsgData      MsgData[MAX_NUM_OF_MSG_CODING_TYPES];
    void ReadIniFile();
    unsigned int MaxNameLength;
    uint32 OldState;
    time_t ltime;

  public:
	  LogFile(char *fileName = "default.log", char *iniFileName = "log.ini");
	  virtual ~LogFile();

	  std::ofstream LogFileOut;
	  virtual void Write();
	  void LogOpenFile();
	  uint8* FindParam(uint8 ParamCode);
};

#endif 
