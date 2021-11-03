# uefi-tool-debug

a little tool used to check your uefi system.  
一个简单的小工具用来访问uefi系统的各种资源。刚接触这方面的东西，希望有疏漏错误的地方多多交流指正，谢谢。  

1.mem(memory)  
  (1)mem  

  (2)mem r(read) hex(address)  

  (3)mem r(read) hex(address) hex(count)  

  (4)mem w(write) hex(address) hex(value)  

2.mmio  
  (1)mmio  
    1)-mmio   
  (2)mmio r hex(address)  
    1)-mmio r 0  
  (3)mmio r hex(address) hex(count)  
    1)-mmio r ffff 20  
  (4)mmio w hex(address) hex(value)  
    1)-mmio w 0 ff  
3.io  
  (1)io  
    1)-io   
  (2)io r hex(address)  
    1)-mmio r 0  
  (3)io r hex(address) hex(count)  
    1)-mmio r ffff 20  
  (4)io w hex(address) hex(value)  
    1)-mmio w 80 aa  
4.pci  
  (1)pci  

  (2)pci r bbddffrr(bus,device,function,register) hex(count)  
  BusNum = 00;  
  DeviceNum = 03;  
  FunctionNum = 00;  
  RegisterIndex = 00;  

  (3)pci w bbddffrr(bus,device,function,register) hex(value)  
  BusNum = 00;  
  DeviceNum = 03;  
  FunctionNum = 00;  
  RegisterOffset = C0;  
  Value = FF;  

5.isa  
  (1)isa r iiiiddddrrrr(index port,data port, register offset)  
  IndexPort = 0070;  
  DataPort = 0071;  
  RegisterOffset = 0000;  

  (2)isa w iiiiddddrrrr(index port,data port,register offset) hex(value)  
  IndexPort = 0070;  
  DataPort = 0071;  
  RegisterOffset = 0050;  
  Value = AA;  

6.msr  
  (1)msr r hex(address)  

  (2)msr w hex(address) hex(value)  
7.smbo  
  (1)smbo h(handle) hex(handle number)  

  (2)smbo t(type) hex(type number) hex(instance number)   

8.kbc  
9.usb  
10.cpuid  
  (1)cpuid r hex(address)  

11.acpi  
  (1)acpi rsdp  

  (2)acpi rsdt  

  (3)acpi xsdt  

12.spd  
13.ls  
  (1)ls var  

  (2)ls bootdev  

  (3)ls pci  

  (4)ls handle  
    1)ls handle  
    2)ls handle guid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)  

  (5)ls devpath  
    1)ls devpath handle(xxxxxxxx)  

  (6)ls disk  

build
编译
1.VS2012_WDX_ENU.iso
2.nasm-2.15.05-installer-x64.exe
3.edk2-UDK2015.zip
  (1)GIT: f0c97b7716ce92f1fc657f87f29bcc9170e5eb1f https://github.com/tianocore/edk2-BaseTools-win32.git
4.set NASM_PREFIX=C:\Program Files\NASM\
5.Edk2Setup.bat --nt32 x64
6.在ShellPkg.dec里的[Includes]中添加"Library"
7.build -t VS2012x86 -a X64 -p MyPkg\MyPkg.dsc
