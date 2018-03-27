#include <string.h>
#include "../kernel/kernelTypes.h"

#ifdef __MICRO_CONTROLER_MEMORY_ACCESS__

void SetUint16(uint8 *addr, uint16 value) {
  addr[0] = (uint8) value;
  addr[1] = (uint8)(value >> 8);
}

void SetUint32(uint8 *addr, uint32 value) {
  addr[0] = (uint8) value;
  addr[1] = (uint8)(value >> 8);
  addr[2] = (uint8)(value >> 16);
  addr[3] = (uint8)(value >> 24);
}

uint16 GetUint16(uint8 *addr) {
  uint16 tmp;
  tmp   = addr[1];
  tmp <<= 8;
  tmp  |= addr[0];
  return tmp;
}

uint32 GetUint32(uint8 *addr) {
  uint32 tmp;
  tmp   = addr[3];
  tmp <<= 8;
  tmp  |= addr[2];
  tmp <<= 8;
  tmp  |= addr[1];
  tmp <<= 8;
  tmp  |= addr[0];
  return tmp;
}

void SetMemPtr(uint8 *addr, uint8 *value){
  memcpy( addr, &value, sizeof(uint8 *));
}

uint8 *GetMemPtr(uint8 *addr){
  uint8 *tmp=0;
  memcpy( &tmp, addr, sizeof(uint8 *));
  return tmp;
}

#else

#endif
