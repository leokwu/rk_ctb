rm -rf test
g++  test_unique_device2.cpp ../utils/slog.cpp -o test -I/usr/include/libusb-1.0/ -I../utils/  -L/usr/lib/x86_64-linux-gnu/  -lusb-1.0
