COMMON_FILE = $(CURRENT_DIR)/../../Build/GTISDK.env
ifndef GTISDKROOT
	-include $(COMMON_FILE)
endif

all:
	if [ ! -e .stamp_installed ] ; then ./build_from_source.sh; fi

clean:
	rm .stamp_installed

deepclean:
	rm -rf .stamp_installed libusb-*

