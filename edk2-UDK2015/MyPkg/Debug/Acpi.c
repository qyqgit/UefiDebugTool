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
#include "Debug.h"

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
  putchar(RsdpPtr->Signature & 0x000000FF);
  putchar((RsdpPtr->Signature & 0x0000FF00) / 16 /16);
  putchar((RsdpPtr->Signature & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((RsdpPtr->Signature & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);

  putchar((RsdpPtr->Signature & 0x000000FF00000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((RsdpPtr->Signature & 0x0000FF0000000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((RsdpPtr->Signature & 0x00FF000000000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((RsdpPtr->Signature & 0xFF0000FF00000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

  Print(L"Checksum        :%x\r\n",RsdpPtr->Checksum);
  Print(L"OemId           :");
  putchar(RsdpPtr->OemId[0]);
  putchar(RsdpPtr->OemId[1]);
  putchar(RsdpPtr->OemId[2]);
  putchar(RsdpPtr->OemId[3]);
  putchar(RsdpPtr->OemId[4]);
  putchar(RsdpPtr->OemId[5]);
  putchar('\n');
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
  putchar(AcpiHeader->Signature & 0x000000FF);
  putchar((AcpiHeader->Signature & 0x0000FF00) / 16 /16);
  putchar((AcpiHeader->Signature & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((AcpiHeader->Signature & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

  Print(L"Length          :%x\r\n", AcpiHeader->Length);
  Print(L"Revision        :%x\r\n",AcpiHeader->Revision);
  Print(L"CheckSum        :%x\r\n",AcpiHeader->Checksum);
  Print(L"OemId           :");
  putchar(AcpiHeader->OemId[0]);
  putchar(AcpiHeader->OemId[1]);
  putchar(AcpiHeader->OemId[2]);
  putchar(AcpiHeader->OemId[3]);
  putchar(AcpiHeader->OemId[4]);
  putchar(AcpiHeader->OemId[5]);
  putchar('\n');

  Print(L"OemTableId      :");
  putchar(AcpiHeader->OemTableId & 0x000000FF);
  putchar((AcpiHeader->OemTableId & 0x0000FF00) / 16 /16);
  putchar((AcpiHeader->OemTableId & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);

  putchar((AcpiHeader->OemTableId & 0x000000FF00000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0x0000FF0000000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0x00FF000000000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0xFF0000FF00000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

  Print(L"OemRevision     :%x\r\n", AcpiHeader->OemRevision);
  Print(L"CreatorId       :");
  putchar(AcpiHeader->CreatorId & 0x000000FF);
  putchar((AcpiHeader->CreatorId & 0x0000FF00) / 16 /16);
  putchar((AcpiHeader->CreatorId & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((AcpiHeader->CreatorId & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

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
  putchar(AcpiHeader->Signature & 0x000000FF);
  putchar((AcpiHeader->Signature & 0x0000FF00) / 16 /16);
  putchar((AcpiHeader->Signature & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((AcpiHeader->Signature & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

  Print(L"Length          :%x\r\n", AcpiHeader->Length);
  Print(L"Revision        :%x\r\n",AcpiHeader->Revision);
  Print(L"CheckSum        :%x\r\n",AcpiHeader->Checksum);
  Print(L"OemId           :");
  putchar(AcpiHeader->OemId[0]);
  putchar(AcpiHeader->OemId[1]);
  putchar(AcpiHeader->OemId[2]);
  putchar(AcpiHeader->OemId[3]);
  putchar(AcpiHeader->OemId[4]);
  putchar(AcpiHeader->OemId[5]);
  putchar('\n');

  Print(L"OemTableId      :");
  putchar(AcpiHeader->OemTableId & 0x000000FF);
  putchar((AcpiHeader->OemTableId & 0x0000FF00) / 16 /16);
  putchar((AcpiHeader->OemTableId & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);

  putchar((AcpiHeader->OemTableId & 0x000000FF00000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0x0000FF0000000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0x00FF000000000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar((AcpiHeader->OemTableId & 0xFF0000FF00000000) / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

  Print(L"OemRevision     :%x\r\n", AcpiHeader->OemRevision);
  Print(L"CreatorId       :");
  putchar(AcpiHeader->CreatorId & 0x000000FF);
  putchar((AcpiHeader->CreatorId & 0x0000FF00) / 16 /16);
  putchar((AcpiHeader->CreatorId & 0x00FF0000) / 16 /16 / 16 /16);
  putchar((AcpiHeader->CreatorId & 0xFF000000) / 16 /16 / 16 /16 / 16 /16);
  putchar('\n');

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

