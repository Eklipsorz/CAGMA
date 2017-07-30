# Target "extract"
# extract all files from initrd.img
# 
# Target "restore"
# restore current files into new initrd
#
# Target "clean"
# remove all files and dirs except for makefile
extract:
	cp ../install.cfg .
	xz -dc ../tmp/initrd.img | cpio -id
restore:
	find . | cpio -c -o | xz -6 --format=lzma > ../output/initrd.img
clean:
	ls | grep -v "makefile" | xargs rm -rf  # remove all files and dirs except for makefile
	rm -rf .* 2> /dev/null			# remove all hidden files and dirs
