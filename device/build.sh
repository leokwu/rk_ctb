rm -rf ctb_device
g++ ctb_device_demo.cpp ../utils/slog.cpp  ctb_device.cpp  -I../utils/  -lrknn_api -ldl -o ctb_device
sync
