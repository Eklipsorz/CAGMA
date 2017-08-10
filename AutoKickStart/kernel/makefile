# Target "extract"
# extract all files from initrd.img
# 
# Target "restore"
# restore current files into new initrd
#
# Target "clean"
# remove all files and dirs except for makefile
extract:
	@cp ../install.cfg .
	@xz -dc ../tmp/initrd.img | cpio -id
restore:
	@#compassion level: 6 compassion method: lzma
	@find . | cpio -c -o | xz -6 --format=lzma > ../output/initrd.img
clean:
	@# remove all files and dirs except for makefile
	@ls | grep -v "makefile" | xargs rm -rf
	@# remove all hidden files and dirs
	@rm -rf .bash_history
	@rm -rf .buildstamp 
	@rm -rf .profile
