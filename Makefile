all:
	echo 'all target not supported'
	exit 1

distclean:
	make -C ctest distclean
	make -C sccglue distclean
	make -C cordova-demo distclean
	rm -rf sqlite-amalgamation-*
