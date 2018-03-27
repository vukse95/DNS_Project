// **************************************************************************
//                        postOffice.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Descriprtsion:
//  This class handles message exchange. It has a array of queues of TMailBox
//  objcets. It handles getting, and adding messages (buffers) in  appropriate
//  mailbox. The number of mailboxes is defined at the time of creation of the 
//  object.
//  class data:
//   TBuffers *Buffers - pointer on object for memoru handling.
//   TMailBox *MailBoxes[255] - array of pointers on mailboxes.
//   uint8 MailBoxesNum - number of mailboxes.
// **************************************************************************
//
// DECRIPTION OF inline FUNCTIONS
// Function: Add
// Parameters: 
//  (in) uint8 mailBoxID - ID, identification number of mailbox where message is
//                         going to be queued. Id is index in array of mailboxes.
//  (in) uint8 *info - pointer on message.
// Return value: none
// Description: 
//  Queues message in appropriate mailbox. In case of not valid mailbox ID, it throws
//  exception. In case of NULL pointer on message, object of TMailBox calss throws
//  exception. Message is queued at the end of list.
//
// **************************************************************************


#ifndef __POST_OFFICE
#define __POST_OFFICE

#include <windows.h>

#ifndef __MAIL_BOX__
#include "../kernel/mailbox.h"
#endif

#ifndef MAX_NUM_OF_MAILBOXES
#define MAX_NUM_OF_MAILBOXES 255
#endif

class TPostOffice {
  protected:
//    TBuffers *Buffers;
    TMailBox *MailBoxes[MAX_NUM_OF_MAILBOXES];  // pokazivaci na redove poruka
    uint8    MailBoxesNum;     // broj redova poruka
    HANDLE   semHandle[MAX_NUM_OF_MAILBOXES];

    

  public:
    TPostOffice(uint8 mailBoxesNum, TBuffers *buffers);
    ~TPostOffice();

    void   Add( uint8 mailBoxID, uint8 *info);
//    void   AddOnHead( uint8 mailBoxID, uint8 *info);
//    void   AddOnTail( uint8 mailBoxID, uint8 *info){Add( mailBoxID, info);};
    uint8 *Get( uint8 mailBoxID);    //uzima sa pocetka reda (head)
    uint8 *GetSync( uint8 &mailBoxID);    //uzima sa pocetka reda (head)
    uint32 GetCount( uint8 mailBoxID);
    void   Purge();
    void   PurgeMailBox(uint8 mailBoxID);
};

#endif

