// **************************************************************************
//                        mailbox.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  List of messages for automate. There are messages that has to be processed
//  by automate. It knows how to handle messages that are in buffers provided
//  by TBuffers class.
//
//  class data:
//  WIN 32 Data:
//  CRITICAL_SECTION CsMailBox - Critical section data for windows, used to lock and 
//                               unlock mailbox.
//  Linux Data:
//  int Proj - for unique semaphore key generation
//  int SemaphoreId - Id of semaphore.
//  static struct sembuf SemUnlock - data needed to unlock semaphore.
//  static struct sembuf SemLock   - data needed to lock semaphore.
//  
//  Data:
//  uint8  *Head - Head of queue of messages.
//  uint8  *Tail - Tail of queue of messages.
//  uint16 Count - Number of messages in mailbox.
//  TBuffers *Buff - Pointer to buffers object to return discareded messages.
// **************************************************************************
//
// DECRIPTION OF inline FUNCTIONS
// Function: AddOnTail
// Parameters:
//  uint8* - pointer on message.
// Return value: none
// Description: 
//  Ques message in list at the end of the list, after all other messages.
//  This is public function, to be used by other objects. In case of NULL
//  pointer it throwsexception.
//
// **************************************************************************

#ifndef __MAIL_BOX__
#define __MAIL_BOX__

#ifdef WIN32
#include <windows.h>
#elif defined(__LINUX__)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

#ifndef __BUFFERS__
#include "../kernel/buffers.h"
#endif

class TMailBox{
  protected:
#ifdef WIN32
    CRITICAL_SECTION CsMailBox;
#elif defined(__LINUX__) 
 //Linux Version
    static int    SemsUsed;
    static int    SemaphoreId;
    struct sembuf SemUnlock;
    struct sembuf SemLock;
#else 
#endif

    uint8  *Head;
    uint8  *Tail;
    uint32 Count;

  TBuffers *Buff;


    void InitilazieSystemDependent();
    void DeinitilazieSystemDependent();

    inline void  MbxLock();
    inline void  MbxUnlock();
    inline uint8 *GetFromHead();
//    inline void  PutOnHead(uint8 *info);
    inline void  PutOnTail(uint8 *info);

  public:
    TMailBox(TBuffers *buffers);
    ~TMailBox();

    uint8* Get();                          //pri radu zabrane prekide

    void Add(uint8 *info);
//    void AddOnTail(uint8 *info) { Add( info); };
//    void AddOnHead(uint8 *info);
// virtual void AddFromInterrupt(uint8 *info);    // samo za Timer

    uint8 *QueryHead();                    // za uporedjivanje poruka koje treba da se izlancaju
    uint32 GetCount();             //broj poruka u mailbox-u
    void   Purge();
};


#endif
