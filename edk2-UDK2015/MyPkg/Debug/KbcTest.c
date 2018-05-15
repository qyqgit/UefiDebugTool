
#include <Uefi.h>  
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>


#define KBC_DATA_PORT    0x60
#define KBC_CMD_PORT     0x64

VOID
KbcTest (
) 
{ 

  UINT8                           Value;
  Value    = 0;

  IoWrite8(0x64,0xAA);
  gBS->Stall(1000 * 1000);
  Value = IoRead8(0x60);
  Print(L"%x\r\n",Value);
  if (Value == 0x55)
    Print (L"kbc self test ok.\n");
  else
    Print (L"kbc self test fail.\n");
  //io w 64 ae


  IoWrite8(0x60,0xEE);
  gBS->Stall(1000 * 1000);
  Value = IoRead8(0x60);
  Print(L"%x\r\n",Value);
  if (Value == 0xEE)
    Print (L"kbc echo test ok.\n");
  else
    Print (L"kbc echo test fail.\n");

  IoWrite8(0x60,0xFF);
  gBS->Stall(1000 * 1000);
  Value = IoRead8(0x60);
  Print(L"%x\r\n",Value);
  if (Value == 0xAA)
    Print (L"kb self test ok.\n");
  else
    Print (L"kb self test fail.\n");
	
}
