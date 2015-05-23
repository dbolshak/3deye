COMMON		=	common
TESTS		=	tests
FUSION		=	fusion
DVO			=	dvo
TST			=	tst

BUILD_TARGETS	=	$(COMMON) $(TESTS) $(DVO) $(FUSION) $(TST)
CLEAN_TARGETS	=	$(BUILD_TARGETS:%=clean-%)

.PHONY: all $(BUILD_TARGETS)
all: $(BUILD_TARGETS)

$(BUILD_TARGETS):
	@$(MAKE) -C $@;

$(TESTS): $(COMMON)

# FIXME - tests should be actually performed before building any apps
$(FUSION): $(TESTS)

.PHONY: $(CLEAN_TARGETS)
clean: $(CLEAN_TARGETS)

$(CLEAN_TARGETS): 
	$(MAKE) -C $(@:clean-%=%) clean	
