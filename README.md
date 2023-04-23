# PSVR2
An exploration of what can be learned from talking to the PSVR2 and the PSVR 2 Sense Controllers via a USB cable.

## RoadMap

- [x] List device interfaces and endpoints.
- [ ] Retrieve gyroscpic data from the Sense Controllers.
- [ ] Retrieve button press from the Sense Controllers.
- [ ] Turn on the lights on the Sense Controllers.
- [ ] Display something on the screen of the PSVR2.
- [ ] Integrate into either/and OpenHMD or monado.

## Requirements
So far, the code base is very small and only requires make, gcc and libusb.

## Current Progress

After pluging in the PSVR headset to a USB-C port and the PSVR2 Sense Controllers to USB-A ports via a UBS-A to USB-C cable, they can be listed as follow by `libusb`:

```bash
$ lsusb | grep Sony
Bus 009 Device 003: ID 054c:0e45 Sony Corp. PlayStation VR2 Sense Controller (L)
Bus 009 Device 002: ID 054c:0e46 Sony Corp. PlayStation VR2 Sense Controller (R)
Bus 007 Device 002: ID 054c:0cde Sony Corp. BillBoard Device
```

Following that, you can compile and run the main application of this project:
```bash
$ make && ./main
gcc -Wall -Werror -fpic -c main.c -o main.o
gcc -lusb-1.0 main.o -o main
Openning device 6
 Device Descriptor: (Length: 18)
  Vendor ID: 0x054c
  Product ID: 0x0e45
  USB Class: 0x00
  USB Subclass: 0x00
  USB Protocol: 0x00
  Max Packet Size 0: 0x40
  Number of possible configurations: 1
  Manufacturer string index: 1
  Product string index: 2
  Serial Number string index: 0
  bcdUSB: 0x0200
  bcdDevice: 0x0100
  Manufacturer: Sony Interactive Entertainment
  Product: PlayStation VR2 Sense Controller (L)
  Sony Corp. PlayStation VR2 Sense Controller (L)
 Config Descriptor:
   Number of interfaces: 3
    Attribute: 192 (Self-Powered 0, Remote Wakeup 0)
    Max Power: 500mA (SuperSpeed mode: 2000mA)

    Interface #0: Length:9 Type:4 Alternate Setting: 0 Class: 1 SubClass: 1 Protocol: 0 SDesc.: 0 Extra Length: 39
   9   36   1   0   1   39   0   1   1   12   36   2   1   1   1   6   1   0   0   0   0   9   36   6   2   1   1   3   0   0   9   36   3   3   1   3   4   2   0

    Interface #1: Length:9 Type:4 Alternate Setting: 0 Class: 1 SubClass: 2 Protocol: 0 SDesc.: 0
    Interface #1: Length:9 Type:4 Alternate Setting: 1 Class: 1 SubClass: 2 Protocol: 0 SDesc.: 0 Extra Length: 18
   7   36   1   1   1   1   0   11   36   2   1   1   2   16   1   128   187   0
     Endpoint 0: OUT #1, ISOCHRONOUS Adaptive Data endpoint, Maximum Packet Size=25088, Extra Length=7 (i:1, j:1, k:0)

    Interface #2: Length:9 Type:4 Alternate Setting: 0 Class: 3 SubClass: 0 Protocol: 0 SDesc.: 0 Extra Length: 9
   9   33   17   1   0   1   34   252   0
     Endpoint 1: IN #4, INTERRUPT Periodic, Maximum Packet Size=16384, Extra Length=0 (i:2, j:0, k:0)
     Endpoint 2: OUT #3, INTERRUPT Periodic, Maximum Packet Size=16384, Extra Length=0 (i:2, j:0, k:1)
Openning device 7
 Device Descriptor: (Length: 18)
  Vendor ID: 0x054c
  Product ID: 0x0e46
  USB Class: 0x00
  USB Subclass: 0x00
  USB Protocol: 0x00
  Max Packet Size 0: 0x40
  Number of possible configurations: 1
  Manufacturer string index: 1
  Product string index: 2
  Serial Number string index: 0
  bcdUSB: 0x0200
  bcdDevice: 0x0100
  Manufacturer: Sony Interactive Entertainment
  Product: PlayStation VR2 Sense Controller (R)
  Sony Corp. PlayStation VR2 Sense Controller (R)
 Config Descriptor:
   Number of interfaces: 3
    Attribute: 192 (Self-Powered 0, Remote Wakeup 0)
    Max Power: 500mA (SuperSpeed mode: 2000mA)

    Interface #0: Length:9 Type:4 Alternate Setting: 0 Class: 1 SubClass: 1 Protocol: 0 SDesc.: 0 Extra Length: 39
   9   36   1   0   1   39   0   1   1   12   36   2   1   1   1   6   1   0   0   0   0   9   36   6   2   1   1   3   0   0   9   36   3   3   1   3   4   2   0

    Interface #1: Length:9 Type:4 Alternate Setting: 0 Class: 1 SubClass: 2 Protocol: 0 SDesc.: 0
    Interface #1: Length:9 Type:4 Alternate Setting: 1 Class: 1 SubClass: 2 Protocol: 0 SDesc.: 0 Extra Length: 18
   7   36   1   1   1   1   0   11   36   2   1   1   2   16   1   128   187   0
     Endpoint 0: OUT #1, ISOCHRONOUS Adaptive Data endpoint, Maximum Packet Size=25088, Extra Length=7 (i:1, j:1, k:0)

    Interface #2: Length:9 Type:4 Alternate Setting: 0 Class: 3 SubClass: 0 Protocol: 0 SDesc.: 0 Extra Length: 9
   9   33   17   1   0   1   34   252   0
     Endpoint 1: IN #4, INTERRUPT Periodic, Maximum Packet Size=16384, Extra Length=0 (i:2, j:0, k:0)
     Endpoint 2: OUT #3, INTERRUPT Periodic, Maximum Packet Size=16384, Extra Length=0 (i:2, j:0, k:1)
Openning device 11
 Device Descriptor: (Length: 18)
  Vendor ID: 0x054c
  Product ID: 0x0cde
  USB Class: 0x11
  USB Subclass: 0x00
  USB Protocol: 0x00
  Max Packet Size 0: 0x40
  Number of possible configurations: 1
  Manufacturer string index: 17
  Product string index: 18
  Serial Number string index: 0
  bcdUSB: 0x0201
  bcdDevice: 0x0102
  Manufacturer: Realtek
  Product: BillBoard Device
  Sony Corp. BillBoard Device
 Config Descriptor:
   Number of interfaces: 1 BillBoard Device
    Attribute: 128 (Self-Powered 0, Remote Wakeup 0)
    Max Power: 500mA (SuperSpeed mode: 2000mA)

    Interface #0: Length:9 Type:4 Alternate Setting: 0 Class: 17 SubClass: 0 Protocol: 0 SDesc.: 18 BOS Descriptor:
  Device 11 has BOS descriptor:
  Length: 5
  Descriptor Type: 15
  Total Length: 22528
  Number of device capabilities: 4
   BOS Capability 0: Length: 7 Descriptor Type 16 Dev Capability Type: 2
    Data:  0  0  0  0
   BOS Capability 1: Length: 20 Descriptor Type 16 Dev Capability Type: 4
    Data:  0  175  79  172  13  0  135  64  238  183  88  38  161  7  215  108  107
   BOS Capability 2: Length: 48 Descriptor Type 16 Dev Capability Type: 13
    Data:  1  1  0  1  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  33  1  0  0  1  255  1  1
   BOS Capability 3: Length: 8 Descriptor Type 16 Dev Capability Type: 15
    Data:  0  5  12  0  0
```

I do not really know what most of this data means, I see a few endpoints in the PSVR2 Sense Controllers that I will now try to access... Wish me luck! (or help me if you think you can help in any capacity :D )

## Thanks

Most of this project exists so far thanks to the documentation of [libusb](https://libusb.sourceforge.io/api-1.0/libusb_io.html), the official [USB 3.0 Specification sheet](http://www.softelectro.ru/usb30.pdf) and ChatGPT to help me reach the information I want efficiently and get simple sample code to get the project going.

