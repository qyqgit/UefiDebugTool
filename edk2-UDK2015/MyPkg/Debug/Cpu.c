#include <Library/BaseLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>

VOID
PrintCpuName (
    )
{
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;
  CHAR8  String1[48];
  CHAR8  String2[48];
  CHAR8  String3[48];

  Eax = Ebx = Ecx = Edx  = 0;

  AsmCpuid (0x80000002, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32 *)(String1 + 0) = Eax;
  *(UINT32 *)(String1 + 4) = Ebx;
  *(UINT32 *)(String1 + 8) = Ecx;
  *(UINT32 *)(String1 + 12) = Edx;
  String1 [16] = 0;
  AsmCpuid (0x80000003, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32 *)(String2 + 0) = Eax;
  *(UINT32 *)(String2 + 4) = Ebx;
  *(UINT32 *)(String2 + 8) = Ecx;
  *(UINT32 *)(String2 + 12) = Edx;
  String2 [16] = 0;
  AsmCpuid (0x80000004, &Eax, &Ebx, &Ecx, &Edx);
  *(UINT32 *)(String3 + 0) = Eax;
  *(UINT32 *)(String3 + 4) = Ebx;
  *(UINT32 *)(String3 + 8) = Ecx;
  *(UINT32 *)(String3 + 12) = Edx;
  String3 [16] = 0;

  Print(L"%a%a%a\r\n", String1, String2, String3);
}

VOID
CpuId (
    IN UINT64 Address
    )
{
  UINT32 Eax;
  UINT32 Ebx;
  UINT32 Ecx;
  UINT32 Edx;
  CHAR8  String[48];

  Eax = Ebx = Ecx = Edx  = 0;

  if (Address == 0) {

  AsmCpuid ((UINT32)Address, &Eax, &Ebx, &Ecx, &Edx);

  *(UINT32 *)(String + 0) = Ebx;
  *(UINT32 *)(String + 4) = Edx;
  *(UINT32 *)(String + 8) = Ecx;
  String [12] = 0;

  Print (L"String:%a\r\n", String);
  Print (L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n", Eax, Ebx, Ecx, Edx);
  Print (L"Eax:%08a Ebx:%08a Ecx:%08a Edx:%08a\r\n", &Eax, &Ebx, &Ecx, &Edx);

  return;

  }
  if (Address == 0x80000002 || Address == 0x80000003 || Address == 0x80000004) {

    AsmCpuid ((UINT32)Address, &Eax, &Ebx, &Ecx, &Edx);

    *(UINT32 *)(String + 0) = Eax;
    *(UINT32 *)(String + 4) = Ebx;
    *(UINT32 *)(String + 8) = Ecx;
    *(UINT32 *)(String + 12) = Edx;
    String [16] = 0;

    if (Address == 0x80000004)
      Print (L"String: %047a\r\n", String);
    else if (Address == 0x80000003)
      Print (L"String: %032a\r\n", String);
    else if (Address == 0x80000002)
      Print (L"String: %016a\r\n", String);
    Print (L"CpuName:");
    PrintCpuName();
    Print (L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n", Eax, Ebx, Ecx, Edx);
    Print (L"Eax:%08a Ebx:%08a Ecx:%08a Edx:%08a\r\n", &Eax, &Ebx, &Ecx, &Edx);
    return;
  }

  AsmCpuid ((UINT32)Address, &Eax, &Ebx, &Ecx, &Edx);
  Print (L"Eax:%08x Ebx:%08x Ecx:%08x Edx:%08x\r\n", Eax, Ebx, Ecx, Edx);

}
