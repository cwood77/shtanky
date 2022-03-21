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
	dirs \
	$(OUT_DIR)/debug/liam.exe \
	$(OUT_DIR)/release/liam.exe \

clean:
	rm -rf bin

dirs:
	@mkdir -p $(OUT_DIR)/debug
	@mkdir -p $(OBJ_DIR)/debug/araceli
	@mkdir -p $(OBJ_DIR)/debug/cmn
	@mkdir -p $(OUT_DIR)/release
	@mkdir -p $(OBJ_DIR)/release/araceli
	@mkdir -p $(OBJ_DIR)/release/cmn

.PHONY: all clean dirs

# ----------------------------------------------------------------------
# liam

LIAM_SRC = \
	src/araceli/ast.cpp \
	src/araceli/lexor.cpp \
	src/araceli/loader.cpp \
	src/araceli/nameUtil.cpp \
	src/araceli/parser.cpp \
	src/araceli/projectBuilder.cpp \
	src/araceli/symbolTable.cpp \
	src/cmn/ast.cpp \
	src/cmn/lexor.cpp \
	src/cmn/pathUtil.cpp \
	src/test.cpp \

LIAM_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(LIAM_SRC)))

$(OUT_DIR)/debug/liam.exe: $(LIAM_DEBUG_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(LIAM_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST)

$(LIAM_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

LIAM_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(LIAM_SRC)))

$(OUT_DIR)/release/liam.exe: $(LIAM_RELEASE_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(LIAM_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST)

$(LIAM_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
