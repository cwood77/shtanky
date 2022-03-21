COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode
RELEASE_CC_FLAGS = -O3 -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

SCRIPTLIB = scriptlib/xcopy-deploy.bat

all: \
	$(OUT_DIR)/debug/liam.exe \
	$(OUT_DIR)/release/liam.exe \

clean:
	rm -rf bin

.PHONY: all clean

# ----------------------------------------------------------------------
# liam

LIAM_SRC = \
	src/test.cpp \

LIAM_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(LIAM_SRC)))

$(OUT_DIR)/debug/liam.exe: $(LIAM_DEBUG_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/debug
	@$(LINK_CMD) -o $@ $(LIAM_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST)

$(LIAM_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/debug/liam
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

LIAM_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(LIAM_SRC)))

$(OUT_DIR)/release/liam.exe: $(LIAM_RELEASE_OBJ)
	$(info $< --> $@)
	@mkdir -p $(OUT_DIR)/release
	@$(LINK_CMD) -o $@ $(LIAM_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST)

$(LIAM_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@mkdir -p $(OBJ_DIR)/release/liam
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
