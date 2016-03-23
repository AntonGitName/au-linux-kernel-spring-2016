INCLUDE_PATH=../vsd_driver/
SHARE_PATH=../../../../vm/share/

vsd_userspace: main.cpp $(INCLUDE_PATH)/vsd_ioctl.h
	g++ -std=c++11 $< -I $(INCLUDE_PATH) -lbsd -o $@
	cp $@ $(SHARE_PATH)

.PHONY: clean

clean:
	rm -rf vsd_userspace
