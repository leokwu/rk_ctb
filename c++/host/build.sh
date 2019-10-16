rm -rf ctb_host
g++  ctb_host.cpp ../utils/slog.cpp -o ctb_host -I/usr/include/libusb-1.0/ -I../utils/  -L/usr/lib/x86_64-linux-gnu/  -lusb-1.0
