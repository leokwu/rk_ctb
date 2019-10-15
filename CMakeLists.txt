cmake_minimum_required(VERSION 3.0.2)

project(rk_ctb)

SET(CMAKE_SYSTEM_NAME Linux)

# set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++)
set(CMAKE_CXX_FLAGS "-std=c++11 -Wno-error")

add_definitions(-g -o2)

#SET(CMAKE_FIND_ROOT_PATH  ${TOOLCHAIN_DIR}
#	${TOOLCHAIN_DIR}/aarch64-linux-gnu/include
#	${TOOLCHAIN_DIR}/aarch64-linux-gnu/lib )

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/host
	${CMAKE_CURRENT_SOURCE_DIR}/device
	${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# link_directories(
#	${CMAKE_CURRENT_SOURCE_DIR}/lib/
# )

set(ENABLE_HOST                 ON  CACHE BOOL "support in linux" FORCE)
set(ENABLE_DEVICE               ON  CACHE BOOL "support in linux" FORCE)

if (ENABLE_DEVICE)
	set (DEVICE_NAME "ctb_device_demo")
	set (DEVICE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/device/ctb_device_demo.cpp 
			${CMAKE_CURRENT_SOURCE_DIR}/device/ctb_device.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/utils/slog.cpp)
	add_executable(${DEVICE_NAME} ${DEVICE_SRCS})
	target_link_libraries(${DEVICE_NAME} rknn_api dl)
endif(ENABLE_DEVICE)

if (ENABLE_HOST)
	set (HOST_NAME "ctb_host_demo")
	set (HOST_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/host/test_unique_device2.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/utils/slog.cpp)
	add_executable(${HOST_NAME} ${HOST_SRCS})
	target_link_libraries(${HOST_NAME} usb-1.0)

endif(ENABLE_HOST)

