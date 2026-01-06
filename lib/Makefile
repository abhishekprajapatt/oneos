obj-y := $(patsubst $(src)/%/,%,$(wildcard $(src)/*/))

all:
	$(foreach lib, $(obj-y), $(MAKE) -C $(lib) || exit 1;)

clean:
	$(foreach lib, $(obj-y), $(MAKE) -C $(lib) clean || exit 1;)

install:
	$(foreach lib, $(obj-y), $(MAKE) -C $(lib) install || exit 1;)

.PHONY: all clean install
