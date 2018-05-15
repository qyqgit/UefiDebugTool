/*
 * SpdRead.c
 *
 *  Created on: 2017Äê11ÔÂ17ÈÕ
 *      Author: red
 */




#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLIb.h>
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"

#define SMBUS_IO_PORT       0x5040
#define SLAVE_ADDR          0xA8
#define PCI_CONFIG_ADDRESS  0x8000FB20//bus 0 dev 31 fun 3 reg 20h
#define PCI_INDEX_IO_PORT   0xCF8
#define PCI_DATA_IO_PORT    0xCFC

UINT32 FindSmBusDev()
{
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Fun;
  UINT32   Addr;

  Addr = 0x80000000;
  for(Bus = 0x00; Bus < 0xFF; Bus++) {
    for(Dev = 0x00; Dev <= 0x1F; Dev++) {
      for(Fun = 0x00; Fun <= 0x07; Fun++) {
        Addr &= 0xFF000000;//clean the address
        Addr |= ((Bus << 16) | (Dev << 11) | (Fun << 8));//set the pci bus dev and fun
        IoWrite32(PCI_INDEX_IO_PORT, Addr);
        if(IoRead32(PCI_DATA_IO_PORT) != 0xFFFFFFFF) {
          Addr = Addr + 0x08;//read class code
          IoWrite32(PCI_INDEX_IO_PORT, Addr);
          if(IoRead32(PCI_DATA_IO_PORT) >> 16 == 0x0C05)//it's Smbus class code
            return Addr - 0x08;
        }
      }
    }
  }
  return Addr;
}

UINT32 GetSmBusIoPort()
{
  UINT32     PciConfigAddr;
  PciConfigAddr = FindSmBusDev();
  //Print(L"%x\r\n",PciConfigAddr);
  IoWrite32(PCI_INDEX_IO_PORT, PciConfigAddr + 0x20);//pci bar5 is io base address
  return IoRead32(PCI_DATA_IO_PORT) & 0xFFFE;
}

VOID
Spd (
  )
{
  UINT32 Index;
  UINT8 SlaveAddr;
  UINT32 SmBusIoPort;
  UINT8                           Temp[256];

  SmBusIoPort = GetSmBusIoPort();
  //Print(L"%x\r\n",SmBusIoPort);

  for(SlaveAddr = 0xA0; SlaveAddr < 0xA0 + 32; SlaveAddr+=2) {
    //Print(L"%x\r\n",SlaveAddr);
    IoWrite8(SmBusIoPort, 0x1E);
    IoWrite8(SmBusIoPort + 0x04, SlaveAddr + 1);
    IoWrite8(SmBusIoPort + 0x03, 0);//read first byte
    IoWrite8(SmBusIoPort + 0x02, 0x48);//read a byte
    while(IoRead8(SmBusIoPort) & 0x01);//waiting for smbus
    if(IoRead8(SmBusIoPort) & 0x04)//detect error bit
      continue;
    for(Index = 0; Index < 0x100; Index++) {
      IoWrite8(SmBusIoPort, 0x1E);
      IoWrite8(SmBusIoPort + 0x04, SlaveAddr + 1);
      IoWrite8(SmBusIoPort + 0x03, (UINT8)Index);// read index byte
      IoWrite8(SmBusIoPort + 0x02, 0x48);//read a byte
      while(IoRead8(SmBusIoPort) & 0x01);
//      if((Index + 1) % 0x10 == 0)
//        Print(L"%02x\r\n",IoRead8(SmBusIoPort + 0x05));
//      else
//        Print(L"%02x ",IoRead8(SmBusIoPort + 0x05));
      Temp[Index] = IoRead8(SmBusIoPort + 0x05);

    }
    DumpHex(2, 0, 256, Temp);
  }

}

