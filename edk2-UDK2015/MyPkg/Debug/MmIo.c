#include <Protocol/CpuIo2.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"

extern EFI_BOOT_SERVICES  *gBS;


VOID
MmMemoryMappedIoWrite (
  IN UINT64 Address,
  IN UINT64 Value
)
{
  EFI_STATUS                          Status;
  EFI_CPU_IO2_PROTOCOL                *CpuIo;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH                     Width;

  CpuIo         = NULL;
  Width         = EfiPciWidthUint8;
  
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **) &CpuIo);
  if (EFI_ERROR (Status)) {
    CpuIo = NULL;
  }
  
  if (CpuIo != NULL) {
  CpuIo->Mem.Write (CpuIo, Width, Address, 1, &Value);
  }
}

VOID
MmMemoryMappedIoRead (
  IN UINT64 Address,
  IN UINT64 Value
)
{
  UINT8                               Num;
  EFI_STATUS                          Status;
  EFI_CPU_IO2_PROTOCOL                *CpuIo;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH                     Width;
  //UINT8                               Count;
  UINT8                               Temp[256];
  UINT16                              j;
  
  Num           = 0xFF;
  CpuIo         = NULL;
  Width         = EfiPciWidthUint8;
  //Count         = 1;
  j             = 0;
  
  if (Value == 0)
    Num = 0xFF;
  else
    Num = (UINT8)Value - 1;
  
  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **) &CpuIo);
  if (EFI_ERROR (Status)) {
    CpuIo = NULL;
  }
  
  if (CpuIo != NULL) {
    while (Num) {
      CpuIo->Mem.Read (CpuIo, Width, Address++, 1, &Value);
      //Print (L"%02x ",Value);
      Temp[j++] = (UINT8)Value;
      //if (Count++ % 0x10 == 0)
        //Print (L"\r\n");
      Num--;
    }
    CpuIo->Mem.Read (CpuIo, Width, Address, 1, &Value);
    //Print (L"%02x\r\n",Value);
    Temp[j++] = (UINT8)Value;
  }
  DumpHex(2, 0, j, Temp);
}
