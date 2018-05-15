#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ShellLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>

extern EFI_BOOT_SERVICES  *gBS;

VOID MmVarList(
)
{
  EFI_GUID                  FoundVarGuid;
  EFI_STATUS                Status;
  CHAR16                    *pFoundVarName;
  UINTN                     NameSize;
  CHAR16                    *InputStr;
  UINT64                    Count;
  UINT64                    CountBak;
  UINT64                    VarNum;
  
  InputStr        = NULL;
  NameSize        = 0;
  pFoundVarName   = NULL;
  pFoundVarName   = AllocateZeroPool(sizeof(CHAR16));
  Count           = 10;
  CountBak        = 10;
  VarNum          = 0;
  
  while (TRUE) {
    
    if (InputStr != NULL) {
      gBS->FreePool (InputStr);
      InputStr = NULL;
    }
    
    if (!Count) {
      ShellPrintEx (-1, -1, L"input linenum or quit:");
      ShellPromptForResponse (ShellPromptResponseTypeFreeform, NULL, (VOID**) &InputStr);
      if (InputStr[0] == 'q')
        break;
      else if (EFI_ERROR (ShellConvertStringToUint64 (InputStr, &Count, TRUE, TRUE))) {
        if (StrCmp(InputStr,L"\0") == 65363 && StrLen(InputStr) == 512)//for detect null string
          Count = CountBak;
        else
          CountBak = Count = 0;
        continue;
      } else
        CountBak = Count;
    }

    Status = gRT->GetNextVariableName (&NameSize, pFoundVarName, &FoundVarGuid);
    if (Status == EFI_BUFFER_TOO_SMALL)
      continue;
    Print (L"Guid:%g    Name:%s\r\n", &FoundVarGuid, pFoundVarName);
    VarNum++;
    Count--;
    if (Status == EFI_NOT_FOUND)
      break;

      
  }
  Print (L"count of variable:%d\r\n", VarNum);
    
}
