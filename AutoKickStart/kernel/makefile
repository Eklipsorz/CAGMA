SHELL:=/bin/bash
extract:
	xz -dc ../initrd.img | cpio -id
initrd:
	find . | cpio -c -o | xz -6 --format=lzma > ../initrd.img.b
clean:
	ls | grep -v "makefile" | xargs rm -rf
#	rm -rf $(ls -I makefile)
