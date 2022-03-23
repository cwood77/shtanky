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
	$(OUT_DIR)/debug/araceli.exe \
	$(OUT_DIR)/release/araceli.exe \

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
# araceli

ARACELI_SRC = \
	src/araceli/codegen.cpp \
	src/araceli/lexor.cpp \
	src/araceli/loader.cpp \
	src/araceli/main.cpp \
	src/araceli/metadata.cpp \
	src/araceli/nameUtil.cpp \
	src/araceli/parser.cpp \
	src/araceli/projectBuilder.cpp \
	src/araceli/symbolTable.cpp \
	src/cmn/ast.cpp \
	src/cmn/lexor.cpp \
	src/cmn/out.cpp \
	src/cmn/pathUtil.cpp \

ARACELI_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(ARACELI_SRC)))

$(OUT_DIR)/debug/araceli.exe: $(ARACELI_DEBUG_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(ARACELI_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST)

$(ARACELI_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

ARACELI_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(ARACELI_SRC)))

$(OUT_DIR)/release/araceli.exe: $(ARACELI_RELEASE_OBJ)
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(ARACELI_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST)

$(ARACELI_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
