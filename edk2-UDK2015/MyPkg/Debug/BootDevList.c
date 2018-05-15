#include <Protocol/DevicePathToText.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Uefi.h>

extern EFI_BOOT_SERVICES  *gBS;

VOID MmBootDev (
)
{

  
  UINT16                             *VariablePtr;
  UINTN                              VariableSize = 0;
  EFI_STATUS                         Status;
  UINTN                              Count;
  EFI_LOAD_OPTION                    *EfiLoadOption;
  UINTN                              OptionSize = 0;
  CHAR16                             a[100];
  UINTN                              Index;
  UINT8                              NameLen;
  EFI_DEVICE_PATH_PROTOCOL           *pDevicePathProcotol;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL   *pDevicePath2TextProtocol;
  CHAR16                             *pStrDevicePath;
  
  EfiLoadOption = NULL;  
  VariablePtr = NULL;
  pStrDevicePath = NULL;
  //get bootcurrent
  Status  = gRT->GetVariable (L"BootCurrent", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
  if (Status == EFI_BUFFER_TOO_SMALL)
     Status = gBS->AllocatePool (EfiBootServicesData, VariableSize, &VariablePtr);
  gBS->SetMem(VariablePtr ,VariableSize, 0);
  Status  = gRT->GetVariable (L"BootCurrent", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
  UnicodeSPrint (a, sizeof (L"Boot1234"), L"Boot%04x", *VariablePtr);
  Print (L"%s\r\n",a);
  // get bootcurrent option.
  Status  = gRT->GetVariable (a, &gEfiGlobalVariableGuid, NULL, &OptionSize, EfiLoadOption);
  if (Status == EFI_BUFFER_TOO_SMALL)
     Status = gBS->AllocatePool (EfiBootServicesData, OptionSize, &EfiLoadOption);
  gBS->SetMem(EfiLoadOption ,OptionSize, 0);
  Status  = gRT->GetVariable (a, &gEfiGlobalVariableGuid, NULL, &OptionSize, EfiLoadOption);
  Print (L"  DeviceName:%s\r\n",((UINT8*)EfiLoadOption+6));
  //get device path
  NameLen = (UINT8)StrLen((UINT16*)EfiLoadOption + 3);
  Status = gBS->LocateProtocol(&gEfiDevicePathToTextProtocolGuid, NULL, (VOID**)&pDevicePath2TextProtocol);
  
  pDevicePathProcotol = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)EfiLoadOption+ 6 + 2*(NameLen+1));

  pStrDevicePath = pDevicePath2TextProtocol->ConvertDevicePathToText (pDevicePathProcotol, TRUE, TRUE);
  Print (L"  DevicePath:%s\r\n",pStrDevicePath);
  //get boot order.
  Status  = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
  Count = VariableSize / sizeof(UINT16);
  Index = Count - 1;
  if (Status == EFI_BUFFER_TOO_SMALL)
     Status = gBS->AllocatePool (EfiBootServicesData, VariableSize, &VariablePtr);
  gBS->SetMem(VariablePtr ,VariableSize, 0);
  Status  = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
  while (Count--)
  {
    UnicodeSPrint (a, sizeof (L"Boot1234"), L"Boot%04x", VariablePtr[ Index - Count]);
    Print (L"%s\r\n",a);
    
    // get boot option.
    Status  = gRT->GetVariable (a, &gEfiGlobalVariableGuid, NULL, &OptionSize, EfiLoadOption);
    if (Status == EFI_BUFFER_TOO_SMALL)
       Status = gBS->AllocatePool (EfiBootServicesData, OptionSize, &EfiLoadOption);
    gBS->SetMem(EfiLoadOption ,OptionSize, 0);
    Status  = gRT->GetVariable (a, &gEfiGlobalVariableGuid, NULL, &OptionSize, EfiLoadOption);
    Print (L"  DeviceName:%s\r\n",((UINT8*)EfiLoadOption+6));
    //get device path
    NameLen = (UINT8)StrLen((UINT16*)EfiLoadOption + 3);
    Status = gBS->LocateProtocol(&gEfiDevicePathToTextProtocolGuid, NULL, (VOID**)&pDevicePath2TextProtocol);
    
    pDevicePathProcotol = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)EfiLoadOption+ 6 + 2*(NameLen+1));

    pStrDevicePath = pDevicePath2TextProtocol->ConvertDevicePathToText (pDevicePathProcotol, TRUE, TRUE);
    Print (L"  DevicePath:%s\r\n",pStrDevicePath);


  }
  gBS->FreePool(pStrDevicePath);
  gBS->FreePool(EfiLoadOption); 
  gBS->FreePool(VariablePtr);
  

}
