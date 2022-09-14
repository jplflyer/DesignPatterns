DEPDIR := .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CXXFLAGS = --std=c++17 -O3 -g ${AUTO_ARGUMENT}
COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

all: directories programs

clean:
	rm -f obj/* bin/* .d/*

directories: obj bin ${DEPDIR}

obj:
	mkdir -p obj

bin:
	mkdir -p bin

${DEPDIR}:
	mkdir -p $@

programs: bin/SimpleExample

bin/%: obj/%.o
	$(CXX) $^ ${LDFLAGS} $(OUTPUT_OPTION)

.PRECIOUS: obj/%.o
obj/%.o : src/%.cpp
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)
