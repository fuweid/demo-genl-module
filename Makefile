obj-m += demo-module.o

build-mod: ## build-mod builds genl module
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean: ## clean clean temp files for module
	@rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
