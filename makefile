COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode
RELEASE_CC_FLAGS = -O3 -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

SCRIPTLIB = scriptlib/xcopy-deploy.bat

debug: \
	dirs \
	$(OUT_DIR)/debug/araceli.exe \
	$(OUT_DIR)/debug/liam.exe \

all: \
	debug \
	$(OUT_DIR)/release/araceli.exe \
	$(OUT_DIR)/release/liam.exe \

clean:
	rm -rf bin

dirs:
	@mkdir -p $(OUT_DIR)/debug
	@mkdir -p $(OBJ_DIR)/debug/araceli
	@mkdir -p $(OBJ_DIR)/debug/liam
	@mkdir -p $(OBJ_DIR)/debug/cmn
	@mkdir -p $(OUT_DIR)/release
	@mkdir -p $(OBJ_DIR)/release/araceli
	@mkdir -p $(OBJ_DIR)/release/liam
	@mkdir -p $(OBJ_DIR)/release/cmn

.PHONY: debug all clean dirs

# ----------------------------------------------------------------------
# cmn

CMN_SRC = \
	src/cmn/ast.cpp \
	src/cmn/commonLexor.cpp \
	src/cmn/commonParser.cpp \
	src/cmn/intel64.cpp \
	src/cmn/lexor.cpp \
	src/cmn/out.cpp \
	src/cmn/pathUtil.cpp \

CMN_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(CMN_SRC)))

$(OUT_DIR)/debug/cmn.lib: $(CMN_DEBUG_OBJ)
	$(info $< --> $@)
	@ar crs $@ $^

$(CMN_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

CMN_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(CMN_SRC)))

$(OUT_DIR)/release/cmn.lib: $(CMN_RELEASE_OBJ)
	$(info $< --> $@)
	@ar crs $@ $^

$(CMN_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# araceli

ARACELI_SRC = \
	src/araceli/codegen.cpp \
	src/araceli/declasser.cpp \
	src/araceli/lexor.cpp \
	src/araceli/loader.cpp \
	src/araceli/main.cpp \
	src/araceli/metadata.cpp \
	src/araceli/nameUtil.cpp \
	src/araceli/projectBuilder.cpp \
	src/araceli/symbolTable.cpp \

ARACELI_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(ARACELI_SRC)))

$(OUT_DIR)/debug/araceli.exe: $(ARACELI_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(ARACELI_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(ARACELI_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

ARACELI_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(ARACELI_SRC)))

$(OUT_DIR)/release/araceli.exe: $(ARACELI_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(ARACELI_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(ARACELI_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# liam

LIAM_SRC = \
	src/liam/astCodeGen.cpp \
	src/liam/availVarPass.cpp \
	src/liam/instrPrefs.cpp \
	src/liam/lexor.cpp \
	src/liam/lir.cpp \
	src/liam/main.cpp \
	src/liam/projectBuilder.cpp \
	src/liam/varCombiner.cpp \
	src/liam/varFinder.cpp \
	src/liam/varGen.cpp \
	src/liam/varSplitter.cpp \

LIAM_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(LIAM_SRC)))

$(OUT_DIR)/debug/liam.exe: $(LIAM_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(LIAM_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(LIAM_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

LIAM_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(LIAM_SRC)))

$(OUT_DIR)/release/liam.exe: $(LIAM_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(LIAM_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(LIAM_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
