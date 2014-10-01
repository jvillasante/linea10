mkdir /dev/usb

mknod /dev/usb/vcom0 c 180 192
mknod /dev/usb/vcom1 c 180 193

# Add other device driver file nodes here
mknod /dev/WiegandOUT c 207 0
mknod /dev/WiegandIN  c 209 0
mknod /dev/usb/lp0 c 180 0
mknod /dev/usb/ttyACM0 c 166 0
mknod /dev/sda1 b 8 1

# DO NOT REMOVE THIS LINE
insmod /lib/modules/vcom.ko
insmod /lib/modules/WiegandOUTDrv.ko
insmod /lib/modules/WiegandINDrv.ko

