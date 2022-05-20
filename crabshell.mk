################################################################################
#
# crabshell
#
################################################################################

CRABSHELL_VERSION = 1.0.0
CRABSHELL_SITE = ./package/crabshell/src
CRABSHELL_SITE_METHOD = local
CRABSHELL_FILES = ./package/crabshell/etc

define CRABSHELL_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define CRABSHELL_INSTALL_TARGET_CMDS
	mkdir $(TARGET_DIR)/etc/crabshell
	cp -r $(CRABSHELL_FILES)/* $(TARGET_DIR)/etc/crabshell/
	$(INSTALL) -D -m 0755 $(@D)/crabshell $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
