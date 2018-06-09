debug说明文档
---------

1.Mem(memory)
(1)Mem

(2)Mem r(read) hex(address)

(3)Mem r(read) hex(address) hex(count)

(4)Mem w(write) hex(address) hex(value)

2.Mmio
(1)Mmio
1)-mmio 
(2)Mmio r hex(address)
1)-mmio r 0
(3)Mmio r hex(address) hex(count)
1)-mmio r ffff 20
(4)Mmio w hex(address) hex(value)
1)-mmio w 0 ff
3.Io
(1)Io
1)-io 
(2)Io r hex(address)
1)-mmio r 0
(3)Io r hex(address) hex(count)
1)-mmio r ffff 20
(4)Io w hex(address) hex(value)
1)-mmio w 80 aa
4.Pci
(1)Pci

(2)Pci r bbddffrr(bus,device,function,register) hex(count)
BusNum = 00;
DeviceNum = 03;
FunctionNum = 00;
RegisterIndex = 00;

(3)Pci w bbddffrr(bus,device,function,register) hex(value)
BusNum = 00;
DeviceNum = 03;
FunctionNum = 00;
RegisterOffset = C0;
Value = FF;

5.Isa
(1)Isa r iiiiddddrrrr(index port,data port, register offset)
IndexPort = 0070;
DataPort = 0071;
RegisterOffset = 0000;

(2)Isa w iiiiddddrrrr(index port,data port,register offset) hex(value)
IndexPort = 0070;
DataPort = 0071;
RegisterOffset = 0050;
Value = AA;


6.Msr
(1)Msr r hex(address)

(2)Msr w hex(address) hex(value)
7.Smbo
(1)Smbo h(handle) hex(handle number)

(2)Smbo t(type) hex(type number) hex(instance number) 

8.Kbc
9.Usb
10.Cpuid
(1)Cpuid r hex(address)

11.Acpi
(1)Acpi rsdp

(2)Acpi rsdt

(3)Acpi xsdt

12.Spd
13.Ls
(1)Ls var

(2)Ls bootdev

(3)Ls pci

(4)Ls handle
1)Ls handle
2)Ls handle guid(xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)

(5)Ls devpath
1)Ls devpath handle(xxxxxxxx)

(6)Ls disk
