CXX=arm-linux-gnueabihf-g++
AR=ar
CXXFLAGS=-Wall -g -O2 -std=c++11
CXX_OPTS=-c -DMPU9250 -DMPU_DEBUG -I../libs/

SOURCES=sensor.c inv_mpu_lib/inv_mpu.c inv_mpu_lib/inv_mpu_dmp_motion_driver.c
OBJECTS=$(SOURCES:.c=.o)

LIB=libMotionSensor.a

%.o: %.c
	$(CXX) $(CXXFLAGS) $(CXX_OPTS) $< -o $@ 
	

$(LIB): $(OBJECTS) 
	$(AR) rcs $(LIB) $(OBJECTS) 

all: $(LIB)

clean:
	rm -rf *.o
	rm -rf inv_mpu_lib/*.o
	rm -rf $(LIB)
