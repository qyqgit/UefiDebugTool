/*
 * Usb.c
 *
 *  Created on: 2017Äê10ÔÂ11ÈÕ
 *      Author: red
 */




#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Usb2HostController.h>
#include <Library/UefiLib.h>
#include <Library/ShellLib.h>

void FindUsbDev(EFI_STATUS Status, UINTN BufferSize, UINT16 BufferIndex,
    UINT16 TableSize, UINT16 LangID, UINT8 StringID, EFI_HANDLE* HandleBuffer,
    EFI_USB_IO_PROTOCOL* UsbIo, UINT16* LangIDTable, CHAR16** String) {

  EFI_USB_DEVICE_DESCRIPTOR DeviceDescriptor;

  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiUsbIoProtocolGuid, NULL,
      &BufferSize, &HandleBuffer);
  if (EFI_ERROR(Status)) {
    Print(L"LocateHandleBuffer fail.\r\n");
  }
  for (BufferIndex = 0; BufferIndex < BufferSize; BufferIndex++) {
    Status = gBS->HandleProtocol(HandleBuffer[BufferIndex],//OpenProtocol?
        &gEfiUsbIoProtocolGuid, (void**) &UsbIo);
    if (EFI_ERROR (Status) || UsbIo == NULL) {
      continue;
    }
    Print(L"Usb Device:%d/%d\r\n", BufferIndex + 1, BufferSize);
    Status = UsbIo->UsbGetSupportedLanguages(UsbIo, &LangIDTable, &TableSize);
    LangID = *LangIDTable;

    Status = UsbIo->UsbGetStringDescriptor(UsbIo, LangID, StringID, String);
    Print(L"  %s\r\n", *String);
    Print(L"    DeviceDescriptor:\r\n");
    Status = UsbIo->UsbGetDeviceDescriptor(UsbIo, &DeviceDescriptor);
    Print(L"    Length:               %x\r\n", DeviceDescriptor.Length);
    Print(L"    DescriptorType:       %x\r\n", DeviceDescriptor.DescriptorType);
    Print(L"    BcdUSB:               %x\r\n", DeviceDescriptor.BcdUSB);
    Print(L"    DeviceClass:          %x\r\n", DeviceDescriptor.DeviceClass);
    Print(L"    DeviceSubClass:       %x\r\n", DeviceDescriptor.DeviceSubClass);
    Print(L"    DeviceProtocol:       %x\r\n", DeviceDescriptor.DeviceProtocol);
    Print(L"    MaxPacketSize:        %x\r\n", DeviceDescriptor.MaxPacketSize0);
    Print(L"    IdVendor:             %x\r\n", DeviceDescriptor.IdVendor);
    Print(L"    IdProduct:            %x\r\n", DeviceDescriptor.IdProduct);
    Print(L"    BcdDevice:            %x\r\n", DeviceDescriptor.BcdDevice);
    Print(L"    StrManufacturer:      %x\r\n", DeviceDescriptor.StrManufacturer);
    Print(L"    StrProduct:           %x\r\n", DeviceDescriptor.StrProduct);
    Print(L"    StrSerialNumber:      %x\r\n", DeviceDescriptor.StrSerialNumber);
    Print(L"    NumConfigurations:    %x\r\n", DeviceDescriptor.NumConfigurations);
    Print(L"-----------------------------------------------\r\n");
    ShellPromptForResponse (ShellPromptResponseTypeAnyKeyContinue, NULL, (VOID**)NULL);
    //    UsbIo->UsbGetConfigDescriptor();
    //    UsbIo->UsbGetInterfaceDescriptor();
  }
  if (HandleBuffer != NULL)
    Status = gBS->FreePool(HandleBuffer);
}

void GetSetUsbControllerStates(EFI_STATUS Status,EFI_USB_HC_STATE State,
    EFI_USB2_HC_PROTOCOL* Usb2Hc) {
  Status = Usb2Hc->GetState(Usb2Hc, &State);
  Print(L"State:%x\r\n", State);
//  State = EfiUsbHcStateHalt;
//  Print(L"%r\r\n", Usb2Hc->SetState(Usb2Hc, State));
//  Print(L"%r\r\n", Usb2Hc->GetState(Usb2Hc, &State));
//  Print(L"%x\r\n", State);
}

VOID
ShowUsbDev (
  )
{
  EFI_USB2_HC_PROTOCOL      *Usb2Hc;
  EFI_USB_HC_STATE          State;
  EFI_USB_PORT_STATUS       PortStatus;
  UINT16                    Index;
  EFI_STATUS                Status;

  UINT16                    BufferIndex;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     BufferSize;

//  EFI_USB_DEVICE_DESCRIPTOR *DeviceDescriptor;
  EFI_USB_IO_PROTOCOL       *UsbIo;

  IN UINT16 LangID;
  IN UINT8 StringID;
  OUT CHAR16 **String;

  OUT UINT16 *LangIDTable;
  OUT UINT16 TableSize;
  Status        = EFI_SUCCESS;
  State         = EfiUsbHcStateMaximum;
  Usb2Hc        = NULL;
  UsbIo         = NULL;
  LangID        = 0;
  StringID      = 2;// 1 2 3
  String        = NULL;
  BufferIndex   = 0;
  HandleBuffer  = NULL;
  BufferSize    = 0;
  LangIDTable   = NULL;
  TableSize     = 0;

  Print(L"-----------------------------------------------\r\n");
  Status = gBS->LocateProtocol(&gEfiUsb2HcProtocolGuid, NULL, &Usb2Hc);

  FindUsbDev(Status, BufferSize, BufferIndex, TableSize, LangID, StringID,
      HandleBuffer, UsbIo, LangIDTable, String);
  Status = gBS->LocateHandleBuffer (
           ByProtocol,
           &gEfiUsb2HcProtocolGuid,
           NULL,
           &BufferSize,
           &HandleBuffer
          );
  if (EFI_ERROR (Status)) {
    Print (L"LocateHandleBuffer fail.\r\n");
  }
  for (BufferIndex = 0; BufferIndex < BufferSize; BufferIndex++) {
    Status = gBS->HandleProtocol (
                   HandleBuffer[BufferIndex],
                   &gEfiUsb2HcProtocolGuid,
                   (VOID **) &Usb2Hc
                  );
    if (EFI_ERROR (Status) || Usb2Hc == NULL) {
      continue;
    }
    Print(L"Usb Controller:%d/%d  ", BufferIndex + 1, BufferSize);
    GetSetUsbControllerStates(Status, State, Usb2Hc);
    for(Index = 0; Index < 255; Index++) {
      Status = Usb2Hc->GetRootHubPortStatus(Usb2Hc, (UINT8)Index , &PortStatus);
      if(Status == EFI_INVALID_PARAMETER)
        break;
      Print(L"  Usb Port%d:\r\n", Index + 1);
      Print(L"    PortStatus:%x\r\n", PortStatus.PortStatus);
    }
  }
  if (HandleBuffer != NULL)
    Status = gBS->FreePool(HandleBuffer);
}
