#include <Library/UefiLib.h>
#include <Base.h>
VOID
ReadMsr (
  UINT64 Address
)
{
  UINT64 Value = AsmReadMsr64((UINT32)Address);
  Print(L"Edx:%08x ", Value >> 32);
  Print(L"Eax:%08x\r\n", Value);
  Print(L"MSR register:%016lx\r\n", Value);
}

VOID
WriteMsr (
  UINT64 Address,
  UINT64 Value
)
{
  AsmWriteMsr64((UINT32) Address, Value);
}
