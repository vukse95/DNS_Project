/*
Msg To:       LAPD_FSM        (0x0e)    Automate ID: 0x00000000, Call ID:
MsgFrom:      TIMER SYSTEM    (0x00)    Automate ID: 0x00000000, Call ID:
Received Msg: L2_T200_EXPIRED (0x01ba), Length: 24,  Coding type:    0x2
0x01 0x32 ........
0x02 0x01 0x00 0x13......
Start Timer:  Lapd_T200 (0)  
Sent Msg:     L2_T200_EXPIRED (0x01ba), Length:  1,  Coding type:    0x2
Stop Timer:   Lapd_T200 (0)  
Automate State: 4 -> 0
___________________________________________________________________________
*/



// LogFile.cpp: implementation of the LogFile class.
//
//////////////////////////////////////////////////////////////////////

#include "LogFile.h"

//converts two ascii digits in one binary digit
unsigned char atob(char *c) {
  switch(*c) {
    case '0':  return 0;
    case '1':  return 1;
    case '2':  return 2;
    case '3':  return 3;
    case '4':  return 4;
    case '5':  return 5;
    case '6':  return 6;
    case '7':  return 7;
    case '8':  return 8;
    case '9':  return 9;
    case 'a': case 'A':  return 10;
    case 'b': case 'B':  return 11;
    case 'c': case 'C':  return 12;
    case 'd': case 'D':  return 13;
    case 'e': case 'E':  return 14;
    case 'f': case 'F':  return 15;
  }
  return 0;
}

//converts array of ascii digits in array of binary digits, leading zeros are mandatory
long ConvertToBin(char *hex){
  long tmp=0;
  char *dd;
  dd = strchr(hex, ' ');
  if(dd!=0) *dd = 0;
  dd = strchr(hex, 'X');
  if( dd!=0) hex = ++dd;
  dd = strchr(hex, 'x');
  if( dd!=0) hex = ++dd;
  
  while(*hex!=0){
    tmp <<= 4;
    tmp |= atob(hex);
    hex++;
  }
  return tmp;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LogFile::LogFile(char *fileName, char *iniFileName){
  memset(AutomateData, 0x00, sizeof(AutomateData));
  memset(MsgData, 0x00, sizeof(MsgData));
  strcpy(FileName, fileName);
  strcpy(IniFileName, iniFileName);
  MaxNameLength = 0;
  ReadIniFile();
  LogFileOut.open( FileName, std::ios::binary | std::ios::trunc );//ios::binary 
  if( LogFileOut.fail())   {
    std::cout << std::endl << " open file error";
    exit(100);
  }
  LogFileOut.close();
}

LogFile::~LogFile(){

}

void LogFile::ReadIniFile(){
  char keyname[20];
  char data[300];
  char *tmp, *info, *msgName;
  uint16 code, automate, coding, msgNum, left;
  int i;
  code = GetPrivateProfileInt( (LPCWSTR) "PROJECT", (LPCWSTR) "numOfAutoamtes", 0, (LPCWSTR) IniFileName);
  for(i=0; i<MAX_NUM_OF_AUTOMATE_TYPES; i++){
    sprintf(keyname, "%d", i);
    GetPrivateProfileString(  (LPCWSTR) "AUTOMATES", (LPCWSTR)keyname,(LPCWSTR)	"UNKNOWN",(LPWSTR) data, 300,	(LPCWSTR) IniFileName);
    left = 0;
    char *tempData = data;
    while(*tempData++ == ' ') left++;
    while(*(data+strlen(data)) == ' ') *data = 0x00;
    strcpy(AutomateData[i].automateName, data);
    if(MaxNameLength < strlen(AutomateData[i].automateName) ){
      MaxNameLength = strlen(AutomateData[i].automateName);
    }
  }
  i=0;
  sprintf(keyname, "%d", i);
  GetPrivateProfileString( (LPCWSTR) "MESSAGES", (LPCWSTR)keyname,(LPCWSTR)	"END",(LPWSTR) data, 300,(LPCWSTR)	IniFileName);
  while(strcmp(data, "END") != 0){
    sprintf(keyname, "%d", i);
    GetPrivateProfileString( (LPCWSTR) "MESSAGES", (LPCWSTR)keyname,	(LPCWSTR) "END",(LPWSTR) data, 300,(LPCWSTR)	IniFileName);
    if(strcmp(data, "END") == 0) break;

    info = strchr(data, ',');
    *info = 0;
    code = (uint16)ConvertToBin(data);

    info++;
    msgName = info;
    info    = strchr(info, ',');
    *info   = 0;

    info++;
    tmp =strchr(info, ',');
    while(tmp != 0){
      *tmp = 0;
      coding = (uint16)atol(info);
      msgNum = MsgData[coding].NumOfMsgs;
      MsgData[coding].NumOfMsgs++;
      MsgData[coding].msgCode[msgNum] = code;
      strcpy(MsgData[coding].msgName[msgNum], msgName);
      info = ++tmp;
      tmp =strchr(info, ',');
      if(MaxNameLength < strlen(msgName) ){
        MaxNameLength = strlen(msgName);
      }
    }
    coding = (uint16)atol(info);
    msgNum = MsgData[coding].NumOfMsgs;
    MsgData[coding].NumOfMsgs++;
    MsgData[coding].msgCode[msgNum] = code;
    strcpy(MsgData[coding].msgName[msgNum], msgName);
    if(MaxNameLength < strlen(msgName) ){
      MaxNameLength = strlen(msgName);
    }
    i++;
  }
  i=0;
  sprintf(keyname, "%d", i);
  GetPrivateProfileString( (LPCWSTR) "TIMERS", (LPCWSTR)keyname,(LPCWSTR)	"END",(LPWSTR) data, 300,(LPCWSTR)	IniFileName);
  while(strcmp(data, "END") != 0){
    sprintf(keyname, "%d", i);
    GetPrivateProfileString( (LPCWSTR) "TIMERS", (LPCWSTR)keyname,(LPCWSTR)	"END",(LPWSTR) data, 300,(LPCWSTR)	IniFileName);
    if(strcmp(data, "END") == 0) break;
    info = strchr(data, ',');
    *info = 0;
    automate = (uint16)atol(data);
    info++;
    tmp = strchr(info, ',');
    *tmp = 0;
    code = (uint16)atol(info);
    tmp ++;
    strcpy(AutomateData[automate].timerName[code], tmp);
    i++;
  }
}

void LogFile::LogOpenFile(){
  LogFileOut.open( FileName, std::ios::binary | std::ios::app);
  if( LogFileOut.fail())   {
    std::cout << std::endl << " open file error";
    exit(100);
  }
//  LogFileOut.fill('0');
//  LogFileOut.width(8);
//  LogFileOut.setf(ios::hex | ios::showbase | ios::internal);
}

uint8* LogFile::FindParam(uint8 ParamCode){
	uint16 pos = 4;
	while(pos < ParamOffset){
		if(LogBuffer[pos]==ParamCode) return &LogBuffer[pos];
		pos += LogBuffer[pos+1]+2;
	}
	return NULL;
}

void LogFile::Write(){

	// HACK! Something is wrong with this function
	return;

  LogOpenFile();
	uint8 *temp;
  char text[100];
	int i, len;
	switch(GetUint16(LogBuffer)){
		case LOG_EVENT_START:
      time(&ltime);
      LogFileOut << ctime(&ltime);
			temp = FindParam(LOG_PARAM_AUTOMATE);
//      LogFileOut.setf(ios::hex, ios::basefield);
			LogFileOut << "Msg To:       " << AutomateData[temp[2]].automateName << " (" ;
      sprintf(text, "0x%02x", (uint32)temp[2]);
      LogFileOut << text << "),   ";
      memset(text, 32, MaxNameLength-strlen(AutomateData[temp[2]].automateName));
      text[MaxNameLength-strlen(AutomateData[temp[2]].automateName)] = 0;
      LogFileOut << text;
      temp = FindParam(LOG_PARAM_OBJECT_ID);
      sprintf(text, "0x%08x", GetUint32(&temp[2]));
			LogFileOut << "Automate ID: " << text  << std::endl;
			temp = FindParam(LOG_PARAM_STATE);
//      sprintf(text, "0x%02x", (uint32)temp[2]);
//			LogFileOut << " Automate State: " << text << endl;
      OldState = (uint32)temp[2];
		break;
		case LOG_MESSAGE_SENT:
			temp = FindParam(LOG_PARAM_MESSAGE);
			temp += 2;
      if(temp[MSG_TO_AUTOMATE] < MAX_NUM_OF_AUTOMATE_TYPES) {
        LogFileOut << "Msg To:       " << AutomateData[temp[MSG_TO_AUTOMATE]].automateName << " (";
        sprintf(text, "0x%02x", (uint32)temp[MSG_TO_AUTOMATE]);
			  LogFileOut  << text << "),   " ;
        memset(text, 32, MaxNameLength-strlen(AutomateData[temp[MSG_TO_AUTOMATE]].automateName));
        text[MaxNameLength-strlen(AutomateData[temp[MSG_TO_AUTOMATE]].automateName)] = 0;
        LogFileOut << text;
      }
      else{
        LogFileOut << "Msg To:       INVALID (" << (uint32)temp[MSG_FROM_AUTOMATE] << ")  " ;
      }
      sprintf(text, "0x%08x",  GetUint32(&temp[MSG_OBJECT_ID_TO]));
			LogFileOut << "Automate ID: " <<text << std::endl;
      i=0;
      if(temp[MSG_INFO_CODING] < MAX_NUM_OF_MSG_CODING_TYPES) {
        while(MsgData[temp[MSG_INFO_CODING]].msgCode[i] != GetUint16(temp+MSG_CODE) ){
          if(MsgData[temp[MSG_INFO_CODING]].msgName[i][0] == 0) break;
          i++;
        }
        sprintf(text, "0x%04x",  GetUint16(temp+MSG_CODE));
        if(*MsgData[temp[MSG_INFO_CODING]].msgName[i] == 0x00){
          LogFileOut << "Sent Msg:     UNKNOWN MESSAGE"<<" (" << text << "),  ";;
          memset(text, 32, MaxNameLength-strlen("UNKNOWN MESSAGE"));
          text[MaxNameLength-strlen("UNKNOWN MESSAGE")-1] = 0;
          LogFileOut << text;
        }
        else {
          LogFileOut << "Sent Msg:     " <<  MsgData[temp[MSG_INFO_CODING]].msgName[i] <<" (" << text << "),  ";
          memset(text, 32, MaxNameLength-strlen(MsgData[temp[MSG_INFO_CODING]].msgName[i]));
          text[MaxNameLength-strlen(MsgData[temp[MSG_INFO_CODING]].msgName[i])] = 0;
          LogFileOut << text;
        }
      }
      else{
        LogFileOut << "Sent Msg:     " <<  "UNKNOWN CODING TYPE(" << GetUint16(temp+MSG_CODE) << ")  ";
      }
			len = GetUint16(temp+MSG_LENGTH);
			LogFileOut << "Length: "<< len<< "  Coding type: " << (int)temp[MSG_INFO_CODING]<<std::endl;
	
      for( i =0; i<MSG_HEADER_LENGTH; i++){
        sprintf(text, "%02x ",  (uint16)temp[i]);
		    LogFileOut << text;
        if( ((i+1)%4) == 0) LogFileOut << "| ";
      }
 			LogFileOut << std::endl;
      for( i =MSG_HEADER_LENGTH; i<MSG_HEADER_LENGTH+len; i++){
        sprintf(text, "%02x ",  (uint16)temp[i]);
		    LogFileOut << text;
        if( ((i+1-MSG_HEADER_LENGTH)%4) == 0) LogFileOut << "| ";
        if( ((i+1-MSG_HEADER_LENGTH)%24) == 0) LogFileOut << std::endl;
      }
      if( (i>MSG_HEADER_LENGTH) && ((i-MSG_HEADER_LENGTH)%24) != 0)LogFileOut << std::endl;
		break;
    case LOG_EVENT_MESSAGE:
			temp = FindParam(LOG_PARAM_MESSAGE);
			temp += 2;
      if(temp[MSG_FROM_AUTOMATE] < MAX_NUM_OF_AUTOMATE_TYPES) {
        LogFileOut << "MsgFrom:      " << AutomateData[temp[MSG_FROM_AUTOMATE]].automateName << " (";
        sprintf(text, "0x%02x", (uint32)temp[MSG_FROM_AUTOMATE]);
			  LogFileOut  << text << "),   " ;
        memset(text, 32, MaxNameLength-strlen(AutomateData[temp[MSG_FROM_AUTOMATE]].automateName));
        text[MaxNameLength-strlen(AutomateData[temp[MSG_FROM_AUTOMATE]].automateName)] = 0;
        LogFileOut << text;
      }
      else{
        LogFileOut << "MsgFrom: INVALID (" << (uint32)temp[MSG_FROM_AUTOMATE] << ")  " ;
      }
      sprintf(text, "0x%08x",  GetUint32(&temp[MSG_OBJECT_ID_FROM]));
			LogFileOut << "Automate ID: " <<text << std::endl;
      i=0;
      if(temp[MSG_INFO_CODING] < MAX_NUM_OF_MSG_CODING_TYPES) {
        while(MsgData[temp[MSG_INFO_CODING]].msgCode[i] != GetUint16(temp+MSG_CODE) ){
          if(MsgData[temp[MSG_INFO_CODING]].msgName[i][0] == 0) break;
          i++;
        }
        sprintf(text, "0x%04x",  GetUint16(temp+MSG_CODE));
 			  LogFileOut << "Received Msg:" <<  MsgData[temp[MSG_INFO_CODING]].msgName[i] <<" (" << text << "),  ";
        memset(text, 32, MaxNameLength-strlen(MsgData[temp[MSG_INFO_CODING]].msgName[i]));
        text[MaxNameLength-strlen(MsgData[temp[MSG_INFO_CODING]].msgName[i])] = 0;
        LogFileOut << text;
      }
      else{
        LogFileOut << "Received Msg:" <<  "UNKNOWN CODING TYPE(" << GetUint16(temp+MSG_CODE) << ")  ";
      }
			len = GetUint16(temp+MSG_LENGTH);
			LogFileOut << "Length: "<< len<< "  Coding type: " << (int)temp[MSG_INFO_CODING]<<std::endl;
	
      for( i =0; i<MSG_HEADER_LENGTH; i++){
        sprintf(text, "%02x ",  (uint16)temp[i]);
		    LogFileOut << text;
        if( ((i+1)%4) == 0) LogFileOut << "| ";
      }
 			LogFileOut << std::endl;
      for( i =MSG_HEADER_LENGTH; i<MSG_HEADER_LENGTH+len; i++){
        sprintf(text, "%02x ",  (uint16)temp[i]);
		    LogFileOut << text;
        if( ((i+1-MSG_HEADER_LENGTH)%4) == 0) LogFileOut << "| ";
        if( ((i+1-MSG_HEADER_LENGTH)%24) == 0) LogFileOut << std::endl;
      }
      if( (i>MSG_HEADER_LENGTH) && ((i-MSG_HEADER_LENGTH)%24) != 0)LogFileOut << std::endl;
		break;
    case LOG_BUFFER:
			temp = FindParam(LOG_PARAM_MESSAGE);
			temp += 2;
      if(temp[MSG_TO_AUTOMATE] < MAX_NUM_OF_AUTOMATE_TYPES) {
        LogFileOut << "Msg To:       " << AutomateData[temp[MSG_TO_AUTOMATE]].automateName << " (";
        sprintf(text, "0x%02x", (uint32)temp[MSG_TO_AUTOMATE]);
			  LogFileOut  << text << "),   " ;
        memset(text, 32, MaxNameLength-strlen(AutomateData[temp[MSG_TO_AUTOMATE]].automateName));
        text[MaxNameLength-strlen(AutomateData[temp[MSG_TO_AUTOMATE]].automateName)] = 0;
        LogFileOut << text;
      }
      else{
        LogFileOut << "Msg To:       INVALID (" << (uint32)temp[MSG_FROM_AUTOMATE] << ")  " ;
      }
      sprintf(text, "0x%08x",  GetUint32(&temp[MSG_OBJECT_ID_TO]));
      LogFileOut << "Automate ID: " <<text << std::endl;
      i=0;
      if(temp[MSG_INFO_CODING] < MAX_NUM_OF_MSG_CODING_TYPES) {
        while(MsgData[temp[MSG_INFO_CODING]].msgCode[i] != GetUint16(temp+MSG_CODE) ){
          if(MsgData[temp[MSG_INFO_CODING]].msgName[i][0] == 0) break;
          i++;
        }
        sprintf(text, "0x%04x",  GetUint16(temp+MSG_CODE));
        if(*MsgData[temp[MSG_INFO_CODING]].msgName[i] == 0x00){
          LogFileOut << "Msg Buffer:   UNKNOWN MESSAGE"<<" (" << text << "),  ";;
          memset(text, 32, MaxNameLength-strlen("UNKNOWN MESSAGE"));
          text[MaxNameLength-strlen("UNKNOWN MESSAGE")-1] = 0;
          LogFileOut << text;
        }
        else {
          LogFileOut << "Msg Buffer:   " <<  MsgData[temp[MSG_INFO_CODING]].msgName[i] <<" (" << text << "),  ";
          memset(text, 32, MaxNameLength-strlen(MsgData[temp[MSG_INFO_CODING]].msgName[i]));
          text[MaxNameLength-strlen(MsgData[temp[MSG_INFO_CODING]].msgName[i])] = 0;
          LogFileOut << text;
        }
      }
      else{
        LogFileOut << "Msg Buffer    " <<  "UNKNOWN CODING TYPE(" << GetUint16(temp+MSG_CODE) << ")  ";
      }
			len = GetUint16(temp+MSG_LENGTH);
      LogFileOut << "Length: "<< len<< "  Coding type: " << (int)temp[MSG_INFO_CODING]<<std::endl;
	
      for( i =0; i<MSG_HEADER_LENGTH; i++){
        sprintf(text, "%02x ",  (uint16)temp[i]);
		    LogFileOut << text;
        if( ((i+1)%4) == 0) LogFileOut << "| ";
      }
      LogFileOut << std::endl;
      for( i =MSG_HEADER_LENGTH; i<MSG_HEADER_LENGTH+len; i++){
        sprintf(text, "%02x ",  (uint16)temp[i]);
		    LogFileOut << text;
        if( ((i+1-MSG_HEADER_LENGTH)%4) == 0) LogFileOut << "| ";
        if( ((i+1-MSG_HEADER_LENGTH)%24) == 0) LogFileOut << std::endl;
      }
      if( (i>MSG_HEADER_LENGTH) && ((i-MSG_HEADER_LENGTH)%24) != 0)LogFileOut << std::endl;
			LogFileOut << "________________________________________________________________________________________"<< std::endl;
  	  break;
    case LOG_START_TIMER :
			temp = FindParam(LOG_PARAM_AUTOMATE);
      i = temp[2];
			temp = FindParam(LOG_PARAM_TIMER_ID);
      LogFileOut << "Start Timer: " << AutomateData[i].timerName[GetUint16(temp+2)] <<" (" << GetUint16(temp+2) << ")  ";
			LogFileOut << std::endl;
      break;
    case LOG_STOP_TIMER:
			temp = FindParam(LOG_PARAM_AUTOMATE);
      i = temp[2];
			temp = FindParam(LOG_PARAM_TIMER_ID);
      
      LogFileOut << "Stop Timer: " << AutomateData[i].timerName[GetUint16(temp+2)] <<" (" << GetUint16(temp+2) << ")  ";
			LogFileOut << std::endl;
      break;
		case LOG_STRING:
			temp = FindParam(LOG_PARAM_STRING);
			LogFileOut << "String: " << &temp[2] << std::endl;
      break;
		case LOG_EVENT_END:
			temp = FindParam(LOG_PARAM_STATE);
			LogFileOut << "State: " << OldState << " -> "<< (uint32)temp[2] << std::endl;
			LogFileOut << "________________________________________________________________________________________"<< std::endl;
		break;

	}//end switch

  LogFileOut.close();
}
