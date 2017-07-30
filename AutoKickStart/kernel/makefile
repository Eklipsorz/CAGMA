extract:
	xz -dc ../initrd.img | cpio -id
initrd:
	find . | cpio -c -o | xz -6 --format=lzma > ../initrd.img.b
clean:
	rm -rf $(ls -I makefile)
