# makefile

# dirs
src_dir := code
libdds_dir := contrib/libdds/code
samples_dir := samples

# include dirs
inc_dirs := -Icode -Icontrib/libdds/code -Icontrib/glm/

# flags
defines := -D'RESOURCE_PATH=""' -fPIC -DPIC
warnings:= -Wall -Wno-unused -Wno-unused-but-set-variable -Wno-sign-compare
cflags  := -pipe $(inc_dirs) $(defines) $(warnings) -fvisibility=default
ldflags := -s -pipe -L. -Xlinker '-rpath=.'
ldlibs  := -lGL -lglut -lGLEW

# objects
framework_srcs := $(shell find $(src_dir) -type f -name "*.cpp")
framework_hdrs := $(shell find $(src_dir) -type f -name "*.h")
framework_deps := $(patsubst $(src_dir)/%.cpp,build/%.d,$(framework_srcs))
framework_objs := $(patsubst $(src_dir)/%.cpp,build/%.cpp.o,$(framework_srcs))

# libdds
libdds_srcs := $(shell find $(libdds_dir) -type f -name "*.c")
libdds_deps := $(patsubst $(libdds_dir)/%.c,build/%.d,$(libdds_srcs))
libdds_objs := $(patsubst $(libdds_dir)/%.c,build/%.c.o,$(libdds_srcs))

# samples
samples_srcs := $(shell find $(samples_dir) -type f -name "*.cpp")
samples_objs := $(patsubst $(samples_dir)/%/main.cpp,%,$(samples_srcs))

.PHONY: all clean framework libdds.so test

all: libframework.so samples

libdds.so: $(libdds_objs)
	@echo 'LD libdds.so'
	@gcc -o libdds.so -shared $(ldflags) $(ldlibs) $?

libframework.so: libdds.so $(framework_objs)
	@echo 'LD libframework.so'
	@gcc -o libframework.so -shared $(ldflags) $(ldlibs) -ldds $(framework_objs)

samples: libframework.so $(samples_objs)

clean:
	rm build/*
	rm libframework.so
	rm libdds.so
	rm basic
	rm sample1

-include $(framework_deps)
-include $(libdds_deps)

build/%.cpp.o: $(src_dir)/%.cpp $(src_dir)/%.h Makefile
	@echo 'CC $< -> $@'
	@g++ $(cflags) -MD -MMD -MP -c $< -o $@

build/%.c.o: $(libdds_dir)/%.c Makefile
	@echo 'CC $< -> $@'
	@gcc $(cflags) -MD -MMD -MP -c $< -o $@

%: $(samples_dir)/%/main.cpp $(framework_hdrs) Makefile
	@echo 'CCLD $< -> $@'
	@g++ $(cflags) $(ldflags) -lframework $< -o $@
