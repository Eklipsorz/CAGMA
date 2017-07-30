extract:
	cp ../install.cfg .
	xz -dc ../initrd.img | cpio -id
initrd:
	find . | cpio -c -o | xz -6 --format=lzma > ../initrd.img.b
clean:
	ls | grep -v "makefile" | xargs rm -rf  # remove all files and dirs except for makefile
	rm -rf .* 2> /dev/null			# remove all hidden files and dirs
