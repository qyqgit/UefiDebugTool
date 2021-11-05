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

typedef struct ACTION{
	CHAR16 Option[128];
	UINT16 AccessTypeAddressType;
}ACTION_t;

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
  UINT32                i;

  ACTION_t action_tb_access[] = {
    {L"mem", MmMemory},
    {L"mmio", MmMemoryMappedIo},
    {L"io", MmIo},
    {L"pci", MmPci},
    {L"isa", MmIsa},
    {L"msr", MmMsr},
    {L"smbo", MmSmBios},
    {L"kbc", MmKbcTest},
    {L"usb", MmUsb},
    {L"cpuid", MmCpuid},
    {L"acpi", MmAcpi},
    {L"spd", MmSpd},
    {L"ls", MmList}
  };
  ACTION_t action_tb_address[] = {
    {L"rsdp", RSDP},
    {L"rsdt", RSDT},
    {L"xsdt", XSDT},
    {L"var", VAR},
    {L"bootdev", BOOTDEV},
    {L"pci", PCI},
    {L"handle", HANDLE},
    {L"devpath", DEVPATH},
    {L"disk", DISK}
  };
  
  AccessType        = -1;
  Read              = TRUE;
  Address           = 0;
  Value             = 0;
  gpGuidData        = NULL;
  i                 = 0;
  
  for(i = 0; i < (sizeof(action_tb_access) / sizeof(ACTION_t)); i++){
	  if(!StrCmp(action_tb_access[i].Option, ArgvA))
		  AccessType = action_tb_access[i].AccessTypeAddressType;
  }
  if (AccessType == -1) {
    Print(L"error.please input mem/mmio/io/pci/isa/smbo/kbc/spd/usb/cpuid/msr/acpi/ls .\r\n");
    return;
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
  for(i = 0; i < (sizeof(action_tb_address) / sizeof(ACTION_t)); i++){
	  if(!StrCmp(action_tb_address[i].Option, ArgvB))
		  Address = action_tb_address[i].AccessTypeAddressType;
  }
  if (!StrCmp (L"r", ArgvB))//read
    Read = TRUE;
  else if (!StrCmp (L"w", ArgvB))//write
    Read = FALSE;
  else if (!StrCmp (L"h", ArgvB))//handle
    Read = TRUE;
  else if (!StrCmp (L"t", ArgvB))//type
    Read = FALSE;
  else {
    if ((StrCmp (L"", ArgvB) != 0) && (Address == 0)) {
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
