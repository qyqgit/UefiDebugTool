
#include <Uefi.h>  
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <IndustryStandard/SmBios.h>
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"
#include "UefiShellDebug1CommandsLib/SmbiosView/LibSmbiosView.h"

extern EFI_GUID gEfiSmbiosTableGuid;

VOID
MmSmBiosReadByHandle (
  IN UINT64 Address,
  IN UINT64  Value
)
{
  EFI_STATUS                    Status;
  SMBIOS_TABLE_ENTRY_POINT      *mSmbiosTable   = NULL;
  SMBIOS_STRUCTURE_POINTER      m_SmbiosStruct;
  SMBIOS_STRUCTURE_POINTER      *mSmbiosStruct = &m_SmbiosStruct;



  UINT16                    Handle1 = (UINT16)Address;
  UINT8                     *Buffer1;
  UINT16                    Length1;
  
  
  UINT16                    *Handle;
  UINT8                     **Buffer;
  UINT16                    *Length;
  
  SMBIOS_STRUCTURE_POINTER  Smbios;
  SMBIOS_STRUCTURE_POINTER  SmbiosEnd;
  UINT8                     *Raw;
  
  
  mSmbiosTable = NULL;

  //
  // Get SMBIOS table from System Configure table
  //
  Status = GetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID**)&mSmbiosTable);

  if (mSmbiosTable == NULL) {
    Print(L"%r.\r\n",Status);
  }
  //
  // Init SMBIOS structure table address
  //
  mSmbiosStruct->Raw  = (UINT8 *) (UINTN) (mSmbiosTable->TableAddress);
  
  //
  //Find the structure by handle
  //
  Handle = &Handle1;
  Length = &Length1;
  Buffer = &Buffer1;
  
  if (*Handle == INVALID_HANDLE) {
    *Handle = mSmbiosStruct->Hdr->Handle;
    Print(L"input handle  error.\r\n");
    return;
  }

  if ((Buffer == NULL) || (Length == NULL)) {
    return;
  }

  *Length       = 0;
  Smbios.Hdr    = mSmbiosStruct->Hdr;
  SmbiosEnd.Raw = Smbios.Raw + mSmbiosTable->TableLength;
  while (Smbios.Raw < SmbiosEnd.Raw) {
    if (Smbios.Hdr->Handle == *Handle) {
      Raw = Smbios.Raw;
      //
      // Walk to next structure
      //
      LibGetSmbiosString (&Smbios, (UINT16) (-1));
      //
      // Length = Next structure head - this structure head
      //
      *Length = (UINT16) (Smbios.Raw - Raw);
      *Buffer = Raw;
      //
      // update with the next structure handle.
      //
      if (Smbios.Raw < SmbiosEnd.Raw) {
        *Handle = Smbios.Hdr->Handle;
      } else {
        *Handle = INVALID_HANDLE;
      }
      Print(L"Handle:%04x Type:%02x Address:%08x Length:%02x\r\n", *Handle - 1, **Buffer, *Buffer, *Length);
      //return DMI_SUCCESS;
    }
    //
    // Walk to next structure
    //
    LibGetSmbiosString (&Smbios, (UINT16) (-1));
  }

  *Handle = INVALID_HANDLE;
  DumpHex(2, 0, *Length, *Buffer);
  return;
}

UINTN
SmbiosTableLength (
  IN SMBIOS_STRUCTURE_POINTER SmbiosTable
  )
{
  CHAR8  *AChar;
  UINTN  Length;

  AChar = (CHAR8 *)(SmbiosTable.Raw + SmbiosTable.Hdr->Length);
  while ((*AChar != 0) || (*(AChar + 1) != 0)) {
    AChar ++;
  }
  Length = ((UINTN)AChar - (UINTN)SmbiosTable.Raw + 2);
  
  return Length;
}

SMBIOS_STRUCTURE_POINTER
GetSmbiosTableFromType (
  IN SMBIOS_TABLE_ENTRY_POINT  *Smbios,
  IN UINT8                     Type,
  IN UINTN                     Index
  )
{
  SMBIOS_STRUCTURE_POINTER SmbiosTable;
  UINTN                    SmbiosTypeIndex;
  
  SmbiosTypeIndex = 0;
  SmbiosTable.Raw = (UINT8 *)(UINTN)Smbios->TableAddress;
  if (SmbiosTable.Raw == NULL) {
    return SmbiosTable;
  }
  while ((SmbiosTypeIndex != Index) || (SmbiosTable.Hdr->Type != Type)) {
    if (SmbiosTable.Hdr->Type == 127) {
      SmbiosTable.Raw = NULL;
      return SmbiosTable;
    }
    if (SmbiosTable.Hdr->Type == Type) {
      SmbiosTypeIndex ++;
    }
    SmbiosTable.Raw = (UINT8 *)(SmbiosTable.Raw + SmbiosTableLength (SmbiosTable));
  }

  return SmbiosTable;
}

VOID
MmSmBiosReadByType(
  IN UINT64 Address,
  IN UINT8  Value
)
{
  EFI_STATUS                    Status;
  SMBIOS_TABLE_ENTRY_POINT      *mSmbiosTable   = NULL;
  SMBIOS_STRUCTURE_POINTER      m_SmbiosStruct;
  SMBIOS_STRUCTURE_POINTER      *mSmbiosStruct = &m_SmbiosStruct;
  
  mSmbiosTable = NULL;

  //
  // Get SMBIOS table from System Configure table
  //
  Status = GetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID**)&mSmbiosTable);

  if (mSmbiosTable == NULL) {
    Print(L"%r.\r\n",Status);
  }
  //
  // Init SMBIOS structure table address
  //
  mSmbiosStruct->Raw  = (UINT8 *) (UINTN) (mSmbiosTable->TableAddress);
  
  //
  //Find the structure by Type
  //
  m_SmbiosStruct = GetSmbiosTableFromType (mSmbiosTable, (UINT8)Address, Value);
  Print(L"Handle:%04x Type:%02x Address:%08x Length:%02x\r\n", *(m_SmbiosStruct.Raw + 2), *m_SmbiosStruct.Raw, m_SmbiosStruct.Raw, SmbiosTableLength (m_SmbiosStruct));
  DumpHex(2, 0, SmbiosTableLength (m_SmbiosStruct), m_SmbiosStruct.Raw);
}
