# use make to compile all projects on linux or mac
# on linux you need the following:
# sudo apt install -y libglu1-mesa-dev freeglut3-dev mesa-common-dev xorg-dev mesa-utils libglfw3-dev libglew-dev libomp-dev

TOPTARGETS := all clean release mrproper

SUBDIRS := $(wildcard */.)
SUBDIRS := $(filter-out VS/. ,$(SUBDIRS))

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
