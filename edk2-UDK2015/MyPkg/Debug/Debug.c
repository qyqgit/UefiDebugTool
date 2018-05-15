//;******************************************************************************
//;*
//;******************************************************************************
//; Revision History:
//;
//; $Log: $
//;
//;------------------------------------------------------------------------------
//;
//; Abstract: A little tool to access Memory,Io and so on.
//;




#include <Protocol/DevicePathToText.h>

#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"

#include "Debug.h"

#define CALC_EFI_PCI_ADDRESS(Bus, Dev, Func, Reg) \
    ((UINT64) ((((UINTN) Bus) << 24) + (((UINTN) Dev) << 16) + (((UINTN) Func) << 8) + ((UINTN) Reg)))
    
EFI_GUID              *gpGuidData;

extern EFI_BOOT_SERVICES  *gBS;


typedef enum {
  MmNothing,
  MmMemory,
  MmMemoryMappedIo,
  MmIo,
  MmPci,
  MmPciExpress,
  MmIsa,
  MmSmBios,
  MmKbcTest,
  MmUsb,
  MmCpuid,
  MmMsr,
  MmList,
  MmAcpi,
  MmSpd
} SHELL_MM_ACCESS_TYPE;


VOID MmHandleDevPath (
  UINT64  Value
)
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH*                    DevicePath;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL    *pDevicePath2TextProtocol;
  CHAR16                              *pStrDevicePath;
  
  Status = gBS->LocateProtocol(&gEfiDevicePathToTextProtocolGuid, NULL, (VOID**)&pDevicePath2TextProtocol);
  Status = gBS->HandleProtocol ((EFI_HANDLE)Value, &gEfiDevicePathProtocolGuid, (VOID **) &DevicePath);
  if (!EFI_ERROR(Status)) {
    pStrDevicePath = pDevicePath2TextProtocol->ConvertDevicePathToText (DevicePath, TRUE, TRUE);
    Print (L"DevPath:%s\r\n", pStrDevicePath);
  } else
    Print (L"%r.\r\n",Status);
  
  
}


VOID
GotoSwitch(
  IN  SHELL_MM_ACCESS_TYPE  AccessType,
  IN  BOOLEAN               Read,
  IN  UINT64                Address,
  IN  UINT64                Value
                ) 
{
  
  switch (AccessType) {
    
    case MmMemory:
      Read ? MmMemoryRead (Address, Value) : MmMemoryWrite (Address, Value);
      break;
      
    case MmIo:
      Read ? MmIoRead (Address, Value) : MmIoWrite (Address, Value);
      break;
      
    case MmMemoryMappedIo:
      Read ? MmMemoryMappedIoRead (Address, Value) : MmMemoryMappedIoWrite (Address, Value);
      break;
      
    case MmPci:
      Read ? MmPciRead (Address, Value) : MmPciWrite (Address, Value);
      break;
      
    case MmIsa:
      Read ? MmIndexIoRead (Address, Value) : MmIndexIoWrite (Address, Value);
      break;
      
    case MmSmBios:
      Read ? MmSmBiosReadByHandle (Address, Value) : MmSmBiosReadByType (Address, Value);
      break;
      
    case MmKbcTest:
      KbcTest();
      break;
      
    case MmSpd:
      Spd();
      break;

    case MmUsb:
      ShowUsbDev();
      break;

    case MmCpuid:
      CpuId(Address);
      break;

    case MmAcpi:
      AcpiShow(Address);
      break;

    case MmMsr:
      Read ? ReadMsr (Address) : WriteMsr (Address, Value);
      break;

    case MmList:
      if (Address == VAR)
        MmVarList();
      else if (Address == BOOTDEV)
        MmBootDev();
      else if (Address == PCI)
        MmPciList();
      else if (Address == HANDLE)
        MmHandle(gpGuidData);
      else if (Address == DEVPATH)
        MmHandleDevPath(Value);
      else if (Address == DISK)
        HdList();
      break;
      
    default:
      break;
  }
}

VOID
DealWithInputStr (
  IN CHAR16* ArgvA,
  IN CHAR16* ArgvB,
  IN CHAR16* ArgvC,
  IN CHAR16* ArgvD
)
{
  SHELL_MM_ACCESS_TYPE  AccessType;
  BOOLEAN               Read;
  UINT64                Address;
  UINT64                Value;
  EFI_GUID              GuidData;

  
  AccessType        = MmPciExpress;
  Read              = TRUE;
  Address           = 0;
  Value             = 0;
  gpGuidData        = NULL;
  
  if (!StrCmp (L"mem", ArgvA))
    AccessType = MmMemory;
  else if (!StrCmp (L"mmio", ArgvA))
    AccessType = MmMemoryMappedIo;
  else if (!StrCmp (L"io", ArgvA))
    AccessType = MmIo;
  else if (!StrCmp (L"pci", ArgvA))
    AccessType = MmPci;
  else if (!StrCmp (L"isa", ArgvA))
    AccessType = MmIsa;
  else if (!StrCmp (L"msr", ArgvA))
    AccessType = MmMsr;
  else if (!StrCmp (L"smbo", ArgvA))
    AccessType = MmSmBios;
  else if (!StrCmp (L"kbc", ArgvA))
    AccessType = MmKbcTest;
  else if (!StrCmp (L"usb", ArgvA))
    AccessType = MmUsb;
  else if (!StrCmp (L"cpuid", ArgvA))
    AccessType = MmCpuid;
  else if (!StrCmp (L"acpi", ArgvA))
    AccessType = MmAcpi;
  else if (!StrCmp (L"spd", ArgvA))
    AccessType = MmSpd;
  else if (!StrCmp (L"ls", ArgvA))
    AccessType = MmList;
  else {
    if (StrCmp (L"", ArgvA) != 0) {
      Print(L"error.please input mem/mmio/io/pci/isa/smbo/kbc/spd/usb/cpuid/msr/acpi/ls .\r\n");
      return;
    }
  }
//specific access type input check
  if (AccessType == MmKbcTest) {//kbc
    ArgvB[0] = '\0';
    ArgvC[0] = '\0';
    ArgvD[0] = '\0';
  }
  if (AccessType == MmSpd) {//spd
    ArgvB[0] = '\0';
    ArgvC[0] = '\0';
    ArgvD[0] = '\0';
  }
  if (AccessType == MmUsb) {//usb
    ArgvB[0] = '\0';
    ArgvC[0] = '\0';
    ArgvD[0] = '\0';
  }
  if (AccessType == MmAcpi) {//acpi
    ArgvC[0] = '\0';
    ArgvD[0] = '\0';
  }
  if (AccessType == MmSmBios) {//smbo h 0 or smbo t 0 0
    if (StrCmp (L"r", ArgvB) == 0 || StrCmp (L"w", ArgvB) == 0) {
    Print(L"not support smbios read/write.please use h/t.\r\n");
    return;
    } else if (StrCmp (L"h", ArgvB) == 0) {
      if (EFI_ERROR (ShellConvertStringToUint64 (ArgvC, &Address, TRUE, TRUE)) && (StrCmp(L"", ArgvC) != 0)) {
        Print(L"input handle error.\r\n");
        return;
      }
      ArgvD[0] = '\0';
    } else if (StrCmp (L"t", ArgvB) == 0) {
      if (EFI_ERROR (ShellConvertStringToUint64 (ArgvC, &Address, TRUE, TRUE))) {
        Print(L"input type error.\r\n");
        return;
      }

      if (EFI_ERROR (ShellConvertStringToUint64 (ArgvD, &Value, TRUE, TRUE))) {
        Print(L"input instance error.\r\n");
        return;
      }
    }
  }

  
  if (AccessType == MmCpuid) {//cpuid r 0
    if (StrCmp (L"w", ArgvB) == 0) {
    Print(L"not support cpuid write.\r\n");
    return;
    } else {
      if (EFI_ERROR (ShellConvertStringToUint64 (ArgvC, &Address, TRUE, TRUE)) && (StrCmp(L"", ArgvC) != 0)) {
        Print(L"input ax error.\r\n");
        return;
      }
      ArgvD[0] = '\0';
    }
  }

  if (!StrCmp (L"r", ArgvB))//read
    Read = TRUE;
  else if (!StrCmp (L"w", ArgvB))//write
    Read = FALSE;
  else if (!StrCmp (L"h", ArgvB))//handle
    Read = TRUE;
  else if (!StrCmp (L"t", ArgvB))//type
    Read = FALSE;
  else if (!StrCmp (L"rsdp", ArgvB))
    Address = RSDP;
  else if (!StrCmp (L"rsdt", ArgvB))
    Address = RSDT;
  else if (!StrCmp (L"xsdt", ArgvB))
    Address = XSDT;
  else if (!StrCmp (L"var", ArgvB))
    Address = VAR;
  else if (!StrCmp (L"bootdev", ArgvB))
    Address = BOOTDEV;
  else if (!StrCmp (L"pci", ArgvB))
    Address = PCI;
  else if (!StrCmp (L"handle", ArgvB))
    Address = HANDLE;
  else if (!StrCmp (L"devpath", ArgvB))
    Address = DEVPATH;
  else if (!StrCmp (L"disk", ArgvB))
    Address = DISK;
  else {
    if (StrCmp (L"", ArgvB) != 0) {
      Print(L"please input r/w/h/t/rsdp/rsdt/xsdt/var/bootdev/pci/handle/devpath/disk .\r\n");
      return;
    }
      
  }
  

//ls access type input check
  if (AccessType == MmList && Address == DISK) {//ls disk
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
  }

  if (AccessType == MmList && Address == DEVPATH) {//ls devpath 00000000
    if (EFI_ERROR(ShellConvertStringToUint64 (ArgvC, &Value, TRUE, TRUE))) {
      Print(L"input handle error.\r\n");
      return;
    } else {
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
    }
  }
  
  if (AccessType == MmList && Address == HANDLE) {//ls handle or ls handle 00000000-0000-0000-0000-000000000000
    if (!EFI_ERROR (ConvertStringToGuid(ArgvC, &GuidData))) {
      gpGuidData = &GuidData;
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
    } else {
      if (StrCmp (L"", ArgvC) != 0) {
        Print(L"input guid error.\r\n");
        return;
      }

    }
  }
  
  if (AccessType == MmList && Address == PCI) {//ls pci
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
  }
  if (AccessType == MmList && Address == BOOTDEV) {//ls bootdev
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
  }
  if (AccessType == MmList && Address == VAR) {//ls var
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
  }
  
//other access type input check
  if (EFI_ERROR (ShellConvertStringToUint64 (ArgvC, &Address, TRUE, TRUE)) && (StrCmp(L"", ArgvC) != 0) || (Read == FALSE) && (StrCmp(L"", ArgvC) == 0)) {
    Print(L"input address error.\r\n");
    return;
  }
  
  if (EFI_ERROR (ShellConvertStringToUint64 (ArgvD, &Value, TRUE, TRUE)) && (StrCmp(L"", ArgvD) != 0) || (Read == FALSE) && (StrCmp(L"", ArgvD) == 0)) {
    Print(L"input value error.\r\n");
    return;
  }


  //Print (L"%s_%s_%s_%s\r\n", ArgvA, ArgvB, ArgvC, ArgvD);
  //Print (L"%x %x %lx %x\r\n", AccessType, Read, Address, Value);
  GotoSwitch(AccessType, Read, Address, Value);
  
  
}

INTN
EFIAPI
ShellAppMain_my (
  IN UINTN Argc,
  IN CHAR16 **Argv
  ) 
{
  UINTN                           Index;
  BOOLEAN                         Complete;
  CHAR16                          *InputStr;

	CHAR16                          ArgvA[100];
	UINT8                           IndexA;
	CHAR16                          ArgvB[100];
	UINT8                           IndexB;
	CHAR16                          ArgvC[100];
	UINT8                           IndexC;
	CHAR16                          ArgvD[100];
	UINT8                           IndexD;

  InputStr      = NULL;
  Complete      = FALSE;



  do {
    //
    // wait user input to modify
    //
    if (InputStr != NULL) {
      gBS->FreePool (InputStr);
      InputStr = NULL;
    }
    ShellPrintEx (-1, -1, L"-");
    ShellPromptForResponse (ShellPromptResponseTypeFreeform, NULL, (VOID**) &InputStr);
    
    if (InputStr != NULL) {
      Index           = 0;
      IndexA          = 0;
      IndexB          = 0;
      IndexC          = 0;
      IndexD          = 0;
      
      ArgvA[0] = '\0';
      ArgvB[0] = '\0';
      ArgvC[0] = '\0';
      ArgvD[0] = '\0';
      
      while (InputStr[Index] == ' ')
        Index++;

      if (InputStr[Index] == 'q' || InputStr[Index] == 'Q') {
        Complete = TRUE;
        Print (L"quit.\r\n");
        continue;
      } else {
        while (InputStr[Index] != ' ' && InputStr[Index] != '\0') {
          ArgvA[IndexA++] = InputStr[Index];
          Index++;
        }
        ArgvA[IndexA++] = '\0';
        if (InputStr[Index] == '\0') {
          DealWithInputStr(ArgvA, ArgvB, ArgvC, ArgvD);
          continue;
        }
          
        while (InputStr[Index] == ' ')
          Index++;
        while (InputStr[Index] != ' ' && InputStr[Index] != '\0') {
          ArgvB[IndexB++] = InputStr[Index];
          Index++;
        }
        ArgvB[IndexB++] = '\0';
        if (InputStr[Index] == '\0') {
          DealWithInputStr(ArgvA, ArgvB, ArgvC, ArgvD);
          continue;
        }
          

        while (InputStr[Index] == ' ')
          Index++;
        while (InputStr[Index] != ' ' && InputStr[Index] != '\0') {
          ArgvC[IndexC++] = InputStr[Index];
          Index++;
        }
        ArgvC[IndexC++] = '\0';
        if (InputStr[Index] == '\0') {
          DealWithInputStr(ArgvA, ArgvB, ArgvC, ArgvD);
          continue;
        }
          
        while (InputStr[Index] == ' ')
          Index++;
        while (InputStr[Index] != ' ' && InputStr[Index] != '\0') {
          ArgvD[IndexD++] = InputStr[Index];
          Index++;
        }
        ArgvD[IndexD++] = '\0';
        DealWithInputStr(ArgvA, ArgvB, ArgvC, ArgvD);
      }

      continue;


    }
    
  }while (!Complete);

  return EFI_SUCCESS;
	
}

int
main(int argc, char *argv[])
{
  UINTN Argc;
  CHAR16 *Argv = NULL;

  Argc = argc;
  AsciiStrToUnicodeStr(*argv,Argv);

  ShellAppMain_my (Argc, &Argv);
}
