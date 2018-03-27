// **************************************************************************
//                        errorObject.cpp  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  This file contains code of TErrorObject used for error handling in OO 
//  environment.
// **************************************************************************


//#include <stdio.h>

#ifdef __NO_ERROR_STREAM__
#else

#ifdef __ARM_CORE__
#include <iostream>
#else
#include <string.h>
#include <iostream>
#endif//__ARM_CORE__

#endif // __NO_ERROR_STREAM__

#include "../kernel/errorObject.h"


#ifdef __NO_EXCEPTIONS__
#ifdef __NO_ERROR_CLASS__
void ThrowErrorFun(){
#else
void ThrowErrorFun( TErrorObject err){
#endif

#ifdef __NO_ERROR_STREAM__
#else
  cout << err;
#endif  
}
#endif

#ifdef __NO_ERROR_CLASS__
#else

// Function: Constructor
// Parameters: none
// Return value: none
TErrorObject::TErrorObject(){
  Line         = 0;
  ErrorCode    = 0;
  LibErrorCode = 0;
  PrevErrObj   = NULL;
}
// Function: Constructor
// Parameters: 
//  (in) uint32 line - number of line (in code) where error arise.
//  (in) char *file - file name where error arise.
//  (in) uint32 errCode - unique error code.
//  (in) uint32 libErrorCode - error code returned by the system 
//                            (in case of API calls)
// Return value: none
// Description: 
//  Initialization of TErrorObject object, it contains data about error that has arise.
TErrorObject::TErrorObject( uint32 line, const char *file, uint32 errCode, uint32 libErrorCode){
//  sprintf( File, "%s", file);
  strcpy(File, (char *)file);
  Line         = line;
  ErrorCode    = errCode;
  LibErrorCode = libErrorCode;
  PrevErrObj   = NULL;
}


// Function: Constructor
// Parameters:
//  (in) uint32 line - number of line (in code) where error arise.
//  (in) char *file - file name where error arise.
//  (in) uint32 errCode - unique error code.
//  (in) TErrorObject *prev - previous object with error data.
// Return value: none
// Description: 
//  Initialization of TErrorObject object, it contains data about error that has arise.
//  This constructor is used when at first place of error handling, error could not
//  be handled at had to be thrown again. Then new object is generated, with new data
//  about error, but old data is also kept.
TErrorObject::TErrorObject( uint32 line, const char *file, uint32 errCode, TErrorObject &prev, uint32 libErrorCode){
//  sprintf( File, "%s", file);
  strcpy(File, (char *)file);
  ErrorCode    = errCode;
  Line         = line;
  PrevErrObj   = &prev;
  LibErrorCode = libErrorCode;
}


// Function: GetLine
// Parameters: none
// Return value:
//  uint32 - number of line where error has arise.
// Description: 
//  Returns number of line where error has arise.
uint32 TErrorObject::GetLine(){
  return(Line);
}


// Function: GetErrorCode
// Parameters: none
// Return value:
//  uint32 - unique error code.
// Description: 
//  Returns unique error code. Each error has its unique code composed of file code,
//  error level ....
uint32 TErrorObject::GetErrorCode(){
  return( ErrorCode);
}


// Function: GetLibErrorCode
// Parameters: none
// Return value:
//  uint32 - system error code.
// Description: 
//  Returns system error code. If error happend in system call (API) then it's error
//  code is saved.
uint32 TErrorObject::GetLibErrorCode(){
  return( LibErrorCode);
}


// Function: GetFile
// Parameters: none
// Return value:
//  char* - pointer to name of module where error arised.
// Description: 
//  Returns name of file where error arised.
char *TErrorObject::GetFile(){
  return( File);
}


// Function: Descructor
// Parameters: none
// Return value: none
// Description: 
//  At the moment does notihing
TErrorObject::~TErrorObject(){
}


#ifdef __NO_ERROR_STREAM__
#else
// Function:
// Parameters:
// Return value:
// Description: 
//  Redefined operator << so it is posible easy to write contest of TErrorObject, 
//  informations about error.
std::ostream& operator<< (std::ostream &os, TErrorObject &err){
  os << std::endl;
  os << "Greska u liniji: " << err.Line;
  os << " modula: " << err.File << std::endl;
  os.fill('0');
  os.width(2);
  os.setf(std::ios::hex, std::ios::basefield);
  os << " Kod Greske: "  << err.ErrorCode << std::endl;
  os << " Dodatni kod: " << err.LibErrorCode << std::endl;
  os.setf(std::ios::dec, std::ios::basefield);
  os << std::endl << "Nasledjena greska: ";
  if( err.PrevErrObj != NULL)  {
    os << *(err.PrevErrObj);
  }
  else os << "nema" << std::endl;
  return os;
}
#endif
#endif //__NO_ERROR_CLASS__
