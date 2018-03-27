// **************************************************************************
//                        timer.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  Provides handling of timers. It uses delta list for storing them until timer
//  expires. Each timer wetaher stopped or not will expire and will be add in
//  mailbox. If timer is stooped it will only be marked as "to be discarded" and
//  handled later. 
//  class data:
//   static TPostOffice *PostOffice - pointer on object for message exchange.
//   static TBuffers *Buffers - pointer on object for memoru handling.
//   uint8 *Dummy - first timer in list wich is never used or outqueued.
//                  It is used to make operations on list easier.
// **************************************************************************


#ifndef __TIMER__
  #define __TIMER__

#ifndef __POST_OFFICE__
  #include "../kernel/postOffice.h"
#endif

void TimerIntHandler(int);

enum TimerResolutionEnum { Timer1ms = 1, Timer10ms = 10, Timer100ms = 100, Timer1s = 1000
};

//const uint16 Timer1ms   =    1;
//const uint16 Timer10ms  =   10;
//const uint16 Timer100ms =  100;
//const uint16 Timer1s    = 1000;

class CTimer {
  private:
    TimerResolutionEnum TimerResolution;
//    uint16 T1sConst;
//    uint16 T100msConst;
//    uint16 T10msConst;
//    uint16 T1msConst;
  protected:  // napomena: postoje jos neki 'clanovi' lokalni za CPP fajl
   static TBuffers *Buffers;
//   uint8  *Dummy;

   virtual void InitTimerSystem(uint16 timerRes);
   virtual void DeinitTimerSystem();

   inline void Add(uint8 *info);
   inline void DisableTimer();
   inline void EnableTimer();
   inline void Purge();

  public:
   static TPostOffice *PostOffice;

   CTimer( TBuffers *buffers, TPostOffice *postOffice, TimerResolutionEnum timerRes = Timer100ms);
   ~CTimer();

//   uint8 *StartTimer(uint32 count, uint8 mailBoxId, uint8 msgTo, uint16 msgSignalID, 
//               uint8 *info, uint16 timerRes = Timer1s);
   void  StartTimer(uint8 *timerMessage, uint8 mailBoxId, uint32 count,
                TimerResolutionEnum timerRes = Timer1s);

   uint8 *RestartTimer(uint8 *timerMessage, uint32 count, TimerResolutionEnum timerRes = Timer1s);
   void  StopTimer(uint8 *timerMessage);
   bool  TimerRunning(uint8 *timerMessage);
   bool  IsEmpty();

#ifdef WIN32
   friend void CALLBACK TimerIntHandler(UINT wTimerID, UINT msg, 
       DWORD dwUser, DWORD dw1, DWORD dw2);  
#else
   friend void TimerIntHandler(int);
#endif
};
#endif
