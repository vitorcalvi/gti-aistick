This is a tool to test the EUSB interface between host CPU and GTI AI chip. The host first sends one test pattern to the AI chip, the AI chip processes the data and sends back the processed results. The host compares the results with the golden file, so as to validate the EUSB interface.

It can validate both USB bridge chip based EUSB interface and direct EUSB connection interface, for all AI chips.

Usage:

       ./eusbTool -d device_node -i test_pattern_file -o output_file -g golden_file -n block_numbers

device_node: for EUSB dongle, it is the new created /dev/sg device node after the dongle is plugged in, for example, /dev/sg2; for direct EUSB connect, it is /dev/mmcblk0


block_numbers: it is the block numbers for one EUSB READ/WRITE command, set it to 2048 to maximize the performance. set it to 128 if the system cannot support bigger numbers.



