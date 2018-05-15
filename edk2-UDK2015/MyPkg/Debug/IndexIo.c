#include <Library/IoLib.h> 
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"
VOID
MmIndexIoWrite (
  IN UINT64 Address,
  IN UINT64 Value
)
{
  UINT64 IndexPort;
  UINT64 DataPort;
  UINT64 Index;
  //Address = iiiiddddrrrr i = indexport d = dataport r = offset
  DataPort  = (Address / 0x10000 - 0x10000 * (Address / 0x100000000));
  IndexPort = (Address / 0x100000000);
  Index     = (Address - IndexPort * 0x100000000 - DataPort * 0x10000);
  //Print (L"%lx %lx %lx\r\n",IndexPort, DataPort, Index);
  IoWrite16((UINT16)IndexPort, (UINT16)Index);
  IoWrite16((UINT16)DataPort, (UINT8)Value);
}
VOID
MmIndexIoRead (
  IN UINT64 Address,
  IN UINT64 Value
)
{
  UINT8                           Num;
  //UINT8                           Count;
  UINT64                          IndexPort;
  UINT64                          DataPort;
  UINT64                          Index;
  UINT8                           Num2;
  UINT8                           Data;
  UINT8                           Temp[256];
  UINT16                          j;
  
  DataPort  = (Address / 0x10000 - 0x10000 * (Address / 0x100000000));
  IndexPort = (Address / 0x100000000);
  Index     = (Address - IndexPort * 0x100000000 - DataPort * 0x10000);
  
  //Count     = 1;
  Num       = Value == 0 ? 0xFF : (UINT8)Value - 1;
  Num2      = Num;
  j         = 0;

  //Print(L"%x_%x_%x\r\n",IndexPort,DataPort,Index);
  

  while (Num) {
    IoWrite16 (IndexPort, (UINT16)Index + Num2 - Num);
    Data = (UINT8)IoRead16 (DataPort);
    //Print (L"%02x ", Data);
    Temp[j++] = Data;
    //if (Count++ % 0x10 == 0)
      //Print (L"\r\n");
    Num--;
  }
  IoWrite16 (IndexPort, (UINT16)Index + Num2);
  //Print (L"%02x \r\n", (UINT8)IoRead16 (DataPort));
  Temp[j++] = (UINT8)IoRead16 (DataPort);
  
  DumpHex(2, 0, j, Temp);
}
