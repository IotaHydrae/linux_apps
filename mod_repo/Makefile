KERN_DIR:=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean

# obj-m+=eeprom.o
obj-m+=test_eep.o
