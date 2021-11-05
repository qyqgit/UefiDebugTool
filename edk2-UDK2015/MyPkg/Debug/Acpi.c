/*
 * Acpi.c
 *
 *  Created on: 2017Äê9ÔÂ29ÈÕ
 *      Author: red
 */

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <IndustryStandard/Acpi10.h>
#include <Protocol/AcpiTable.h>
#include "UefiShellDebug1CommandsLib/UefiShellDebug1CommandsLib.h"
#include <stdio.h>
#include <string.h>
#include "Debug.h"

VOID PrintEx(VOID* Data, UINT32 Length){
  CHAR16                                    Buffer16[128];
  CHAR8                                     Buffer[128];
  memset(Buffer, 0, sizeof(Buffer));
  memset(Buffer16, 0, sizeof(Buffer16));
  strncpy(Buffer, (CHAR8*)Data, Length);
  AsciiStrToUnicodeStr(Buffer, Buffer16);
  Print(L"%s\r\n", Buffer16);
}

VOID
AcpiShow (
  IN UINT64        Address
  )
{
  EFI_STATUS                                Status;
  EFI_ACPI_DESCRIPTION_HEADER               *AcpiHeader;
  VOID                                      *AcpiTable;
  EFI_ACPI_6_0_ROOT_SYSTEM_DESCRIPTION_POINTER *RsdpPtr;
  UINTN                                     EntryIndex;
  UINT8                                     Brk;

  AcpiTable = NULL;
  Brk       = 1;

  Status = EfiGetSystemConfigurationTable (
             &gEfiAcpi20TableGuid,
             &AcpiTable
             );
  if (EFI_ERROR (Status)) {
    Status = EfiGetSystemConfigurationTable (
               &gEfiAcpi10TableGuid,
               &AcpiTable
               );
  }
  //
  // We do not ASSERT if AcpiTable not found. It is possbile that a platform does not produce AcpiTable.
  //
  if (AcpiTable == NULL) {
    Print(L"acpi table is not found.\r\n");
  }
  RsdpPtr = (EFI_ACPI_6_0_ROOT_SYSTEM_DESCRIPTION_POINTER*)AcpiTable;

  switch(Address) {
    case RSDP :
      goto pRSDP;
      break;
    case RSDT :
      goto pRSDT;
      break;
    case XSDT :
      goto pXSDT;
      break;
  }

pRSDP:  Print (L"-----------------------------------------------------\r\n");
  Print(L"RSDP Address    :%08x\r\n", AcpiTable);
  Print(L"Signature       :");
  PrintEx(&RsdpPtr->Signature, sizeof(RsdpPtr->Signature));
  Print(L"Checksum        :%x\r\n",RsdpPtr->Checksum);
  Print(L"OemId           :");
  PrintEx(&RsdpPtr->OemId[0], sizeof(RsdpPtr->OemId));
  Print(L"Revision        :%x\r\n",RsdpPtr->Revision);
  Print(L"RsdtAddress     :%x\r\n",RsdpPtr->RsdtAddress);
  Print(L"Length          :%x\r\n",RsdpPtr->Length);
  Print(L"XsdtAddress     :%x\r\n",RsdpPtr->XsdtAddress);
  Print(L"ExtendedChecksum:%x\r\n",RsdpPtr->ExtendedChecksum);
  Print(L"Reserved        :%02x ",RsdpPtr->Reserved[0]);
  Print(L"%02x ",RsdpPtr->Reserved[1]);
  Print(L"%02x\r\n",RsdpPtr->Reserved[2]);

//  DumpHex(2, 0, 256, AcpiTable);
  return;
pRSDT:  Print (L"-----------------------------------------------------\r\n");
  AcpiHeader = (EFI_ACPI_DESCRIPTION_HEADER*) RsdpPtr->RsdtAddress;
  Print(L"RSDT Address    :%08x\r\n", RsdpPtr->RsdtAddress);
  Print(L"Signature       :");
  PrintEx(&AcpiHeader->Signature, sizeof(AcpiHeader->Signature));

  Print(L"Length          :%x\r\n", AcpiHeader->Length);
  Print(L"Revision        :%x\r\n",AcpiHeader->Revision);
  Print(L"CheckSum        :%x\r\n",AcpiHeader->Checksum);
  Print(L"OemId           :");
  PrintEx(&AcpiHeader->OemId[0], sizeof(AcpiHeader->OemId));
  Print(L"OemTableId      :");
  PrintEx(&AcpiHeader->OemTableId, sizeof(AcpiHeader->OemTableId));

  Print(L"OemRevision     :%x\r\n", AcpiHeader->OemRevision);
  Print(L"CreatorId       :");
  PrintEx(&AcpiHeader->CreatorId, sizeof(AcpiHeader->CreatorId));

  Print(L"CreatorRevision :%x\r\n",AcpiHeader->CreatorRevision);
  for(EntryIndex = 0; EntryIndex < (AcpiHeader->Length - 36) / 4; EntryIndex++) {
    Print(L"Entry[%02x]:%08x\r\n", EntryIndex, *((UINTN*)(EntryIndex * 4 + (UINTN)AcpiHeader + 36)));
    if(Brk % 10 == 0)
      ShellPromptForResponse (ShellPromptResponseTypeAnyKeyContinue, NULL, (VOID**)NULL);
    Brk++;
  }
//  DumpHex(2, (UINTN)AcpiHeader, 256, AcpiHeader);
  return;
pXSDT:  Print (L"-----------------------------------------------------\r\n");
  AcpiHeader = (EFI_ACPI_DESCRIPTION_HEADER*) RsdpPtr->XsdtAddress;
  Print(L"XSDT Address    :%08x\r\n", RsdpPtr->RsdtAddress);
  Print(L"Signature       :");
  PrintEx(&AcpiHeader->Signature, sizeof(AcpiHeader->Signature));

  Print(L"Length          :%x\r\n", AcpiHeader->Length);
  Print(L"Revision        :%x\r\n",AcpiHeader->Revision);
  Print(L"CheckSum        :%x\r\n",AcpiHeader->Checksum);
  Print(L"OemId           :");
  PrintEx(&AcpiHeader->OemId[0], sizeof(AcpiHeader->OemId));

  Print(L"OemTableId      :");
  PrintEx(&AcpiHeader->OemTableId, sizeof(AcpiHeader->OemTableId));
  Print(L"OemRevision     :%x\r\n", AcpiHeader->OemRevision);
  Print(L"CreatorId       :");
  PrintEx(&AcpiHeader->CreatorId, sizeof(AcpiHeader->CreatorId));
  Print(L"CreatorRevision :%x\r\n",AcpiHeader->CreatorRevision);
  for(EntryIndex = 0; EntryIndex < (AcpiHeader->Length - 36) / 8; EntryIndex++) {
    Print(L"Entry[%02x]:%016lx\r\n", EntryIndex, *((UINTN*)(EntryIndex * 8 + (UINTN)AcpiHeader + 36)));
    if(Brk % 10 == 0)
      ShellPromptForResponse (ShellPromptResponseTypeAnyKeyContinue, NULL, (VOID**)NULL);
    Brk++;
  }
//  DumpHex(2, (UINTN)AcpiHeader, 256, AcpiHeader);
  return;

}

