# ESP8266-NodeMcu-XVC-Server
A working Xilinx Virtual Cable server built on ESP8266.
This driver works flawlessly with Xilinx ISE IMPact and Vivado HW Server.
I also created a driver within OPENOCD framework, a working fork may be found at
https://github.com/tamood/riscv-openocd

# Usage
1) Upload the firmware on a ESP-12E/F compatible board. Pinout may be changed in the enum PINS . Note down acquired IP as espIP.
2) Use following in Xilinx ISE-IMPact/Vivado as cable plugins.

xilinx_xvc host=espIP:2542 maxpacketsize=256 disableversioncheck=true 

3) Use OpenOCD as follows
openocd -c "interface xilinx_virtual_cable; xvc_host espIP"

# Attribution
This code is inspired from following work.

https://github.com/gtortone/esp-xvcd
