// **************************************************************************
//                        errorObject.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  Used for error handling in OO environment. It caries code and place of error
//  to the place of error handling.
//
//  class data:
//  uint32 ErrorCode    - 32 bit code of error.
//  uint32 LibErrorCode - 32 bit system error code.
//  uint32 Line         - Line where error happend.
//  char   File[256]    - File where error happend.
//  TErrorObject *PrevErrObj - Previous error object that handled message, previous
//                             level of error handling.
// **************************************************************************



#ifndef __ERROR_OBJECT__
 #define __ERROR_OBJECT__


#ifndef __KERNEL_TYPES__
#include "kernelTypes.h"
#endif

#ifdef __NO_ERROR_STREAM__
#else
#ifdef __ARM_CORE__
  #include <iostream>
#else
  #include <iostream>
#endif
#endif

#ifdef __NO_ERROR_CLASS__
#else

class TErrorObject{
  protected:
    uint32 ErrorCode;
    uint32 LibErrorCode;
    uint32 Line;
    char   File[256];

    TErrorObject *PrevErrObj;

  public:
    TErrorObject();
    TErrorObject( uint32 line, const char *file, uint32 errCode = 0, uint32 libErrrCode = 0);
    TErrorObject( uint32 line, const char *file, uint32 errCode, TErrorObject &prev, uint32 libErrrCode = 0);
    ~TErrorObject();

    uint32 GetLine();
    uint32 GetErrorCode();
    uint32 GetLibErrorCode();
    char   *GetFile();

#ifdef __NO_ERROR_STREAM__
#else
    friend std::ostream & operator<< (std::ostream &os, TErrorObject &err);
#endif    
};

#endif  // __NO_ERROR_CLASS__
#endif //__ERROR_OBJECT__

#ifdef __NO_EXCEPTIONS__

#ifdef __NO_ERROR_CLASS__
#define TErrorObject 
void ThrowErrorFun();
#define ThrowError(x) ThrowErrorFun()
#else
void ThrowErrorFun( const TErrorObject err);
#define ThrowError(x) ThrowErrorFun(x)
#endif

#else

#ifdef __NO_ERROR_CLASS__
#define ThrowError(x) throw 
#else
#define ThrowError(x) throw x
#endif


#endif

