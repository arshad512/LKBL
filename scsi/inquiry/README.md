# Background

Bus connects and help transfer information between two components within a computer. A bus internally is made up of data, memory and control lines or paths. Data path transfers Mbytes between components. This is further qualified by memory/address path. The control lines determin the direction of the transfer.

Bus which connects between CPU and IO device are called IO Bus. Example, SCSI.

SCSI is detailed under :-
1. SAM = SSCI-3 Architecture Model (Fundamantal SCSI Architecture)
2. SPC = SCSI primary commands, defines the commands and their behaviour
3. SBC = SCSI Block commands (for disk)



This section demonstrate how to invoke a SCSI command using sg utils and 
programitically. The SCSI command used in this example is INQUIRY (0x12).

INQUIRY is one of the mandatory command set under SCSI-3 and must be 
implemented. Inquiry Returns LUN specific information. The program is borrowed
from sg_utils site (https://github.com/hreinecke/sg3_utils/blob/master/examples/scsi_inquiry.c)
with slight modifications.


With sginfo (sg3_utils) the output is like below.
```
$ sginfo /dev/sda2
INQUIRY response (cmd: 0x12)
---------------------------------------
Device Type                0
Vendor:                    ATA
Product:                   VBOX HARDDISK
Revision level:            1.0
```

With code (scsi_inquiry.c), the output is similar, only the display format is different.
```
$ gcc -Wall -o sginfo scsi_inquiry.c
$ ./sginfo
ATA       VBOX HARDDISK     1.0 , byte_7=0x2
$
```
