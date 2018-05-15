#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <Base.h>

#define   VAR           255
#define   BOOTDEV       254
#define   PCI           253
#define   HANDLE        252
#define   DEVPATH       251
#define   DISK          250
#define   RSDP          249
#define   RSDT          248
#define   XSDT          247

//#include <Uefi.h>  
//#include <Library/UefiLib.h>
//#include <Library/ShellCEntryLib.h>
//#include <Library/ShellLib.h>
//#include <Protocol/PciRootBridgeIo.h>
//#include <Library/UefiBootServicesTableLib.h>
//#include <Library/UefiRuntimeServicesTableLib.h>
//#include <Protocol/CpuIo2.h>
//#include <Library/BaseLib.h>
//#include <Library/IoLib.h> 
//#include <Library/MemoryAllocationLib.h>
//#include <Library/DebugLib.h>
//#include <Library/PrintLib.h>
//#include <IndustryStandard/Pci22.h>
//#include <Library/ShellCommandLib.h>
//#include "UefiShellDebug1CommandsLib/Pci.h"


VOID
MmMemoryRead (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
MmMemoryWrite (
  IN UINT64 Address,
  IN UINT64 Value);
  
VOID
MmIoRead (
  IN UINT64 Address,
  IN UINT64 Value
);
VOID
MmIoWrite (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
MmMemoryMappedIoRead (
  IN UINT64 Address,
  IN UINT64 Value
);
VOID
MmMemoryMappedIoWrite (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
MmPciRead (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
MmPciWrite (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
MmIndexIoRead (
  IN UINT64 Address,
  IN UINT64 Value
);
VOID
MmIndexIoWrite (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID MmVarList(
);

VOID MmBootDev (
);

VOID MmHandle (
  IN EFI_GUID  *gpGuidData
);

VOID MmPciList(
);

VOID
HdList (
    );

VOID
KbcTest(
    );

VOID
MmSmBiosReadByHandle (
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
MmSmBiosReadByType(
  IN UINT64 Address,
  IN UINT64 Value
);

VOID
CpuId (
    IN UINT64 Address
    );

VOID
ReadMsr (
    IN UINT64 Address
    );

VOID
WriteMsr (
    IN UINT64 Address,
    IN UINT64 Value
    );

VOID
AcpiShow (
  IN UINT64        Address
  );

VOID
ShowUsbDev (
    );
VOID
Spd (
    );
#endif
