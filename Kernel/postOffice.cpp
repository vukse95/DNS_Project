// **************************************************************************
//                        postoOfice.cpp  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Descriprtsion:
//   This file contains code for class TPostOffice which handles message exchange.
// **************************************************************************

#ifdef WIN32
#include <windows.h>
#endif

//#include <stdio.h>
//#include <fstream.h>

#include "../kernel/postOffice.h"
#include "../kernel/errorObject.h"


// Function: Constructor
// Parameters: 
//  (in) uint8 mailBoxesNum - number of mailboxes to be initialized.
//  (in) TBuffers *buffers - pointer on object for memory handling.
// Return value: none
// Description: 
//  Initialization of TPostOffice object, it is object used for messages exchange
//  between automates.
TPostOffice::TPostOffice(uint8 mailBoxesNum, TBuffers *buffers){
  int i;
  //validate parameteres
#ifndef __NO_PARAMETER_CHECK__
  if( mailBoxesNum == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  if( buffers == NULL)   ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  //initalize data
  MailBoxesNum = mailBoxesNum;
//  Buffers = buffers;


  //create mail boxes needed for system
  for (i=0; i<mailBoxesNum; i++) {
    MailBoxes[i] = new TMailBox( buffers);
    if(MailBoxes[i] == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
    semHandle[i] = CreateSemaphore( NULL, 0, 10000, NULL);
    if( semHandle[i] == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010222));

  }//end for i
  //rest pointers to mail boxes initalize to NULL
  for (i=mailBoxesNum; i<MAX_NUM_OF_MAILBOXES; i++) MailBoxes[i] = NULL;
}


// Ime Funkcije:       AddFromInterrupt
// Ulazni parametri:   uint8 mailBoxID - ID (Indetifikacioni broj) mailbox-a u koji ce se
//                                       poruka ulancati, tj. njegov index u nizu mailbox-ova
//                     uint8 *info     - pokazivac na poruku koju treba ulancati
//                     uint8           - parametar se ne koristi, postoji radi kompatibilnosti
//                                       test verzijom kernela
// Izlazni parametari: nema
// Opis: Ulancava poruku u odgovarajuci mailbox. U slucaju da se prosledi kod mailbox-a
//       koji ne postoji generise se izuazetak (exception). Provera NULL pokazivaca na
//       poruku se vrsi u objektu klase TMailBox. Koristi se iz kriticnih sekcija jer
//       ne zabranjuje prekide.

//void TPostOffice::AddFromInterrupt( uint8 mailBoxID, uint8 *info, uint8 )
//{
// if(mailBoxID >= MailBoxesNum) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
//
// MailBoxes[mailBoxID]->AddFromInterrupt( info);
//}


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
void TPostOffice::Add( uint8 mailBoxID, uint8 *info){
#ifndef __NO_PARAMETER_CHECK__
  if(mailBoxID >= MailBoxesNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif  
  MailBoxes[mailBoxID]->Add( info);
  ReleaseSemaphore(semHandle[mailBoxID], 1, NULL);
}

/*
// Function: AddOnHead
// Parameters: 
//  (in) uint8 mailBoxID - ID, identification number of mailbox where message is
//                         going to be queued. Id is index in array of mailboxes.
//  (in) uint8 *info - pointer on message.
// Return value: none
// Description: 
//  Queues message in appropriate mailbox. In case of not valid mailbox ID, it throws
//  exception. In case of NULL pointer on message, object of TMailBox calss throws
//  exception. It queues message at the begining of list, it means that it will be 
//  first for processing.
void TPostOffice::AddOnHead( uint8 mailBoxID, uint8 *info ){
  if(mailBoxID >= MailBoxesNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  MailBoxes[mailBoxID]->AddOnHead( info);
}
*/

// Function: Get
// Parameters: 
//  (in) uint8 mailBoxID - ID, identification number of mailbox from where message is
//                         going to be outqueued. Id is index in array of mailboxes.
// Return value:
//  uint8* - pointer on message.
// Description: 
//  Outqueues message in from appropriate mailbox. In case of not valid mailbox ID, 
//  it throws exception. In case there is no more messages in queue it returns
//  NULL pointer.
uint8 *TPostOffice::Get( uint8 mailBoxID){
#ifndef __NO_PARAMETER_CHECK__
  if(mailBoxID >= MailBoxesNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  return(MailBoxes[mailBoxID]->Get());
}

uint8 *TPostOffice::GetSync( uint8 &mailBoxID){
  DWORD dwWaitResult = WaitForMultipleObjects( MailBoxesNum, semHandle, FALSE, INFINITE);
  mailBoxID = (uint8)(dwWaitResult-WAIT_OBJECT_0);
#ifndef __NO_PARAMETER_CHECK__
  if(mailBoxID >= MailBoxesNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  return(MailBoxes[mailBoxID]->Get());
}

// Function: GetCount
// Parameters: 
//  (in) uint8 mailBoxID - ID, identification number of mailbox.
// Return value:
//  uint32  - number of messages in mailbox.
// Description: 
//  Returns number of messages currently in mailbox. In case of not walid mailbox ID
//  it throws exception.
uint32 TPostOffice::GetCount( uint8 mailBoxID ){
#ifndef __NO_PARAMETER_CHECK__
  if(mailBoxID >= MailBoxesNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif  
  return(MailBoxes[mailBoxID]->GetCount());
}


// Function: Purge
// Parameters: none
// Return value: none
// Description: 
//  Deletes all messages, from all mailboxes.
void TPostOffice::Purge(){
  for (uint8 i=0; i<MailBoxesNum; i++)  PurgeMailBox(i);
}


// Function: PurgeMailBox
// Parameters:
//  (in) uint8 mailBoxID - ID, identification number of mailbox from where message is
//                         going to be outqueued. Id is index in array of mailboxes.
// Return value: none
// Description: 
//  Deletes all messages, from mailboxes. In case of not valid mailbox ID, 
//  it throws exception.
void TPostOffice::PurgeMailBox(uint8 mailBoxID){
#ifndef __NO_PARAMETER_CHECK__
  if(mailBoxID >= MailBoxesNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  MailBoxes[mailBoxID]->Purge();
}


// Function: Destructor
// Parameters:   none
// Return value: none
// Description: 
//  Deletes all messages, from mailboxes.
TPostOffice::~TPostOffice(){
  Purge();
  for (int i=0; i<MailBoxesNum; i++) {
    delete MailBoxes[i];
    CloseHandle(semHandle[i]);
  }
}


