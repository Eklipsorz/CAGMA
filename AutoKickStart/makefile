all: ./tmp/initrd.img ./tmp/vmlinuz extract
mirror:=http://mirror01.idc.hinet.net/CentOS/6/os/x86_64/isolinux

# Variable mirror stores URL 

# Download vmlinuz and initrd.img
./tmp/initrd.img:
	wget ${mirror}/initrd.img -P ./tmp
./tmp/vmlinuz:
	wget ${mirror}/vmlinuz -P ./tmp
extract: ./tmp/initrd.img ./tmp/vmlinuz
	make -C kernel/ $@
restore:
	make -C kernel/ $@
clean:
	rm -rf tmp/
	make -C kernel/ $@
