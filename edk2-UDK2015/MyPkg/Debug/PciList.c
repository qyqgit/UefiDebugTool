#include <Protocol/PciRootBridgeIo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>
#include <IndustryStandard/Pci22.h>
#include "UefiShellDebug1CommandsLib/Pci.h"
#include <Uefi.h>

extern EFI_BOOT_SERVICES  *gBS;


/**
  This function get the next bus range of given address space descriptors.
  It also moves the pointer backward a node, to get prepared to be called
  again.

  @param[in, out] Descriptors Points to current position of a serial of address space
                              descriptors.
  @param[out] MinBus          The lower range of bus number.
  @param[out] MaxBus          The upper range of bus number.
  @param[out] IsEnd           Meet end of the serial of descriptors.

  @retval EFI_SUCCESS     The command completed successfully.
**/
EFI_STATUS
PciGetNextBusRange (
  IN OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR  **Descriptors,
  OUT    UINT16                             *MinBus,
  OUT    UINT16                             *MaxBus,
  OUT    BOOLEAN                            *IsEnd
  );

/**
  This function gets the protocol interface from the given handle, and
  obtains its address space descriptors.

  @param[in] Handle          The PCI_ROOT_BRIDIGE_IO_PROTOCOL handle.
  @param[out] IoDev          Handle used to access configuration space of PCI device.
  @param[out] Descriptors    Points to the address space descriptors.

  @retval EFI_SUCCESS     The command completed successfully
**/
EFI_STATUS
PciGetProtocolAndResource (
  IN  EFI_HANDLE                            Handle,
  OUT EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL       **IoDev,
  OUT EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR     **Descriptors
  );

VOID PrintVidDid(UINT64 Address) {
  EFI_STATUS Status;
  UINT16 Vid;
  UINT16 Did;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH Width;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL* IoDev;
  EFI_HANDLE* HandleBuffer;
  UINTN BufferSize;
  UINTN Index;
  IoDev = NULL;
  HandleBuffer = NULL;
  BufferSize = 0;
  Width = EfiPciWidthUint16;
  Vid = 0;
  Did = 0;
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPciRootBridgeIoProtocolGuid,
      NULL, &BufferSize, &HandleBuffer);
  if (EFI_ERROR(Status)) {
    Print(L"LocateHandleBuffer fail.\r\n");
  }
  for (Index = 0; Index < BufferSize; Index++) {
    Status = gBS->HandleProtocol(HandleBuffer[Index],
        &gEfiPciRootBridgeIoProtocolGuid, (void*) &IoDev);
    if (EFI_ERROR(Status)) {
      continue;
    }
  }
  if (IoDev != NULL) {
    Status = IoDev->Pci.Read(IoDev, Width, Address, 1, &Vid);
    Print(L"  Vid:%02x ", Vid);
    Address = Address + 2;
    Status = IoDev->Pci.Read(IoDev, Width, Address, 1, &Did);
    Print(L"Did:%02x\r\n", Did);
  }
  if (HandleBuffer != NULL) {
    gBS->FreePool(HandleBuffer);
  }
}

VOID MmPciList(
)
{
  UINT16                            Bus;
  UINT16                            Device;
  UINT16                            Func;
  UINT64                            Address;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *IoDev;
  EFI_STATUS                        Status;
  UINTN                             HandleBufSize;
  EFI_HANDLE                        *HandleBuf;
  UINTN                             HandleCount;
  UINTN                             Index;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *Descriptors;
  UINT16                            MinBus;
  UINT16                            MaxBus;
  BOOLEAN                           IsEnd;
  PCI_COMMON_HEADER                 PciHeader;
  UINT8                             Brk;
  
  Brk     = 1;

  //
  // Get all instances of PciRootBridgeIo. Allocate space for 1 EFI_HANDLE and
  // call LibLocateHandle(), if EFI_BUFFER_TOO_SMALL is returned, allocate enough
  // space for handles and call it again.
  //
  HandleBufSize = sizeof (EFI_HANDLE);
  HandleBuf     = (EFI_HANDLE *) AllocateZeroPool (HandleBufSize);
  if (HandleBuf == NULL) {
    Print (L"not enouth memory.\r\n"); 
    return;
  }

  Status = gBS->LocateHandle (
                ByProtocol,
                &gEfiPciRootBridgeIoProtocolGuid,
                NULL,
                &HandleBufSize,
                HandleBuf
               );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    HandleBuf = ReallocatePool (sizeof (EFI_HANDLE), HandleBufSize, HandleBuf);
    if (HandleBuf == NULL) {
      Print (L"not enouth memory.\r\n");  
      return;
    }

    Status = gBS->LocateHandle (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &HandleBufSize,
                  HandleBuf
                 );
  }

  if (EFI_ERROR (Status)) {
    Print (L"no found.\r\n"); 
    return;
  }

  HandleCount = HandleBufSize / sizeof (EFI_HANDLE);

  //
  // For each handle, which decides a segment and a bus number range,
  // enumerate all devices on it.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = PciGetProtocolAndResource (
              HandleBuf[Index],
              &IoDev,
              &Descriptors
             );
    if (EFI_ERROR (Status)) {
      Print (L"not found.\r\n");
      return;
    }
    //
    // No document say it's impossible for a RootBridgeIo protocol handle
    // to have more than one address space descriptors, so find out every
    // bus range and for each of them do device enumeration.
    //
    while (TRUE) {
      Status = PciGetNextBusRange (&Descriptors, &MinBus, &MaxBus, &IsEnd);

      if (EFI_ERROR (Status)) {
        Print (L"not found.\r\n");
        return;
      }

      if (IsEnd) {
        break;
      }

      for (Bus = MinBus; Bus <= MaxBus; Bus++) {
        //
        // For each devices, enumerate all functions it contains
        //
        for (Device = 0; Device <= PCI_MAX_DEVICE; Device++) {
          //
          // For each function, read its configuration space and print summary
          //
          for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {

            Address = CALC_EFI_PCI_ADDRESS (Bus, Device, Func, 0);
            IoDev->Pci.Read (
                        IoDev,
                        EfiPciWidthUint16,
                        Address,
                        1,
                        &PciHeader.VendorId
                       );

            //
            // If VendorId = 0xffff, there does not exist a device at this
            // location. For each device, if there is any function on it,
            // there must be 1 function at Function 0. So if Func = 0, there
            // will be no more functions in the same device, so we can break
            // loop to deal with the next device.
            //
            if (PciHeader.VendorId == 0xffff && Func == 0) {
              break;
            }

            if (PciHeader.VendorId != 0xffff) {



              IoDev->Pci.Read (
                          IoDev,
                          EfiPciWidthUint32,
                          Address,
                          sizeof (PciHeader) / sizeof (UINT32),
                          &PciHeader
                         );

              Print (L"Bus:%02x Dev:%02x Fun:%02x  ", Bus, Device, Func);
              Print (L"Clas:%02x SubClas:%02x Inter:%02x\r\n",PciHeader.ClassCode[2], PciHeader.ClassCode[1], PciHeader.ClassCode[0]);
              PrintVidDid(Bus * 0x1000000 + Device * 0x10000 + Func * 0x100);
              if(Brk % 10 == 0)
                ShellPromptForResponse (ShellPromptResponseTypeAnyKeyContinue, NULL, (VOID**)NULL);
              Brk++;
              //
              // If this is not a multi-function device, we can leave the loop
              // to deal with the next device.
              //
              if (Func == 0 && ((PciHeader.HeaderType & HEADER_TYPE_MULTI_FUNCTION) == 0x00)) {
                break;
              }
            }
          }
        }
      }
      //
      // If Descriptor is NULL, Configuration() returns EFI_UNSUPPRORED,
      // we assume the bus range is 0~PCI_MAX_BUS. After enumerated all
      // devices on all bus, we can leave loop.
      //
      if (Descriptors == NULL) {
        break;
      }
    }
  }
  if (HandleBuf != NULL) {
    FreePool (HandleBuf);
  }
}
