#include <Protocol/PciRootBridgeIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"
#include <Uefi.h>
extern EFI_BOOT_SERVICES  *gBS;


VOID
MmPciWrite (
  IN UINT64 Address, //Address = bbddffrr
  IN UINT64 Value
)
{
  EFI_STATUS                      Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           BufferSize;
  UINTN                           Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH                    Width;
  
  IoDev         = NULL;
  HandleBuffer  = NULL;
  BufferSize    = 0;
  Width         = EfiPciWidthUint8;
  

  Status = gBS->LocateHandleBuffer (
           ByProtocol,
           &gEfiPciRootBridgeIoProtocolGuid,
           NULL,
           &BufferSize,
           &HandleBuffer
          );
  if (EFI_ERROR (Status)) {
    Print (L"LocateHandleBuffer fail.\r\n");
  }
  for (Index = 0; Index < BufferSize; Index++) {
    Status = gBS->HandleProtocol (
                   HandleBuffer[Index],
                   &gEfiPciRootBridgeIoProtocolGuid,
                   (VOID *) &IoDev
                  );
    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  if (IoDev != NULL) {
    IoDev->Pci.Write (IoDev, Width, Address, 1, &Value);
  }
  
  if (HandleBuffer != NULL) {
  gBS->FreePool (HandleBuffer);
  }
}

VOID
MmPciRead (
  IN UINT64 Address,
  IN UINT64 Value
)
{
  EFI_STATUS                      Status;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH         Width;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *IoDev;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           BufferSize;
  UINTN                           Index;
  UINT8                           Num;
  //UINT8                           Count;
  UINT8                           Temp[256];
  UINT16                          j;
  
  //Count         = 1;
  IoDev         = NULL;
  HandleBuffer  = NULL;
  BufferSize    = 0;
  Width         = EfiPciWidthUint8;
  j             = 0;
  
  Num = Value == 0 ? 0xFF : (UINT8)Value -1;

  

  Status = gBS->LocateHandleBuffer (
           ByProtocol,
           &gEfiPciRootBridgeIoProtocolGuid,
           NULL,
           &BufferSize,
           &HandleBuffer
          );
  if (EFI_ERROR (Status)) {
    Print (L"LocateHandleBuffer fail.\r\n");
  }
  for (Index = 0; Index < BufferSize; Index++) {
    Status = gBS->HandleProtocol (
                   HandleBuffer[Index],
                   &gEfiPciRootBridgeIoProtocolGuid,
                   (VOID *) &IoDev //(VOID **) &IoDev
                  );
    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  if (IoDev != NULL) {
    while (Num) {
      IoDev->Pci.Read (IoDev, Width, Address++, 1, &Value);
      //Print (L"%02x ",Value);
      Temp[j++]= (UINT8)Value;
      //if (Count++ % 0x10 == 0)
        //Print (L"\r\n");
      Num--;
    }
    Status = IoDev->Pci.Read (IoDev, Width, Address, 1, &Value);
    //Print (L"%02x\r\n",Value);
    Temp[j++] = (UINT8)Value;
  }
  DumpHex(2, 0, j, Temp);
  if (HandleBuffer != NULL) {
  gBS->FreePool (HandleBuffer);
  }
  
}
