#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>

extern EFI_BOOT_SERVICES  *gBS;


VOID MmHandle (
  IN EFI_GUID  *gpGuidData
)
{
  EFI_HANDLE      *HandleBuffer;
  UINTN           HandleCount;
  EFI_STATUS      Status;
  UINTN           HandleIndex;
  UINTN           LineNum;
  CHAR16          *InputStr;
  UINT64          PageCount;
  UINT64          PageCountBak;
  
  LineNum       = 0;
  InputStr      = NULL;
  PageCount     = 10;
  PageCountBak  = 10;
  HandleIndex   = 0;
  
  if (gpGuidData == NULL) {//list all handles
    Status = gBS->LocateHandleBuffer (
    AllHandles,
    NULL,
    NULL,
    &HandleCount,
    &HandleBuffer
    );
    if (!EFI_ERROR (Status)) {
      for (HandleIndex; HandleIndex < HandleCount; HandleIndex++) {
        Print (L"HandleIndex:%04d  Handle:%08x\r\n", HandleIndex, HandleBuffer[HandleIndex]);
        LineNum++;
        if (LineNum % PageCount == 0) {
A:        ShellPrintEx (-1, -1, L"input linenum or quit:");
          ShellPromptForResponse (ShellPromptResponseTypeFreeform, NULL, (VOID**) &InputStr);
          if (InputStr[0] == 'q')
            break;
          else if (EFI_ERROR (ShellConvertStringToUint64 (InputStr, &PageCount, TRUE, TRUE))) {
            if (StrCmp(InputStr,L"\0") == 65363 && StrLen(InputStr) == 512)//for detect null string
              PageCount = PageCountBak;
            else {
              goto A;
            }
            
          } else if (PageCount <= 0) {
            PageCount = PageCountBak;
          }
          else {
            PageCountBak = PageCount;
          }
          LineNum = 0;
        }
      }
      Print (L"count of handle:%d\r\n", HandleIndex + 1);
    }
  } else {
    Status = gBS->LocateHandleBuffer (
    ByProtocol,
    gpGuidData,
    NULL,
    &HandleCount,
    &HandleBuffer
    );
    if (Status == EFI_NOT_FOUND)
      Print (L"not found handle.\r\n");
    if (!EFI_ERROR (Status)) {
      for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
        Print (L"HandleIndex:%04d  Handle:%08x\r\n", HandleIndex, HandleBuffer[HandleIndex]);
      }
    }
    
  }
  
  if (HandleBuffer != NULL) {
  gBS->FreePool (HandleBuffer);
  }
  
  
}
