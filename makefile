COMPILE_CMD = x86_64-w64-mingw32-g++
LINK_CMD = x86_64-w64-mingw32-g++
OBJ_DIR = bin/obj
OUT_DIR = bin/out
DEBUG_CC_FLAGS = -ggdb -c -Wall -D cdwDebugMode
RELEASE_CC_FLAGS = -O3 -c -Wall
DEBUG_LNK_FLAGS_POST = -ggdb -static-libgcc -static-libstdc++ -static
RELEASE_LNK_FLAGS_POST = -static-libgcc -static-libstdc++ -static

SCRIPTLIB = scriptlib/xcopy-deploy.bat

dtest: debug
	@cmd /c appr.bat

rtest: all
	@cmd /c appr.bat rel

atest: dtest rtest

debug: \
	dirs \
	$(OUT_DIR)/debug/appr.exe \
	$(OUT_DIR)/debug/araceli.exe \
	$(OUT_DIR)/debug/liam.exe \
	$(OUT_DIR)/debug/philemon.exe \
	$(OUT_DIR)/debug/salome.exe \
	$(OUT_DIR)/debug/shlink.exe \
	$(OUT_DIR)/debug/shtasm.exe \
	$(OUT_DIR)/debug/shtemu.exe \

all: \
	debug \
	$(OUT_DIR)/release/appr.exe \
	$(OUT_DIR)/release/araceli.exe \
	$(OUT_DIR)/release/liam.exe \
	$(OUT_DIR)/release/philemon.exe \
	$(OUT_DIR)/release/salome.exe \
	$(OUT_DIR)/release/shlink.exe \
	$(OUT_DIR)/release/shtasm.exe \
	$(OUT_DIR)/release/shtemu.exe \

clean:
	rm -rf bin

dirs:
	@mkdir -p $(OBJ_DIR)/debug/appr
	@mkdir -p $(OBJ_DIR)/debug/araceli
	@mkdir -p $(OBJ_DIR)/debug/cmn
	@mkdir -p $(OBJ_DIR)/debug/liam
	@mkdir -p $(OBJ_DIR)/debug/philemon
	@mkdir -p $(OBJ_DIR)/debug/salome
	@mkdir -p $(OBJ_DIR)/debug/shlink
	@mkdir -p $(OBJ_DIR)/debug/shtasm
	@mkdir -p $(OBJ_DIR)/debug/shtemu
	@mkdir -p $(OBJ_DIR)/debug/syzygy
	@mkdir -p $(OBJ_DIR)/release/appr
	@mkdir -p $(OBJ_DIR)/release/araceli
	@mkdir -p $(OBJ_DIR)/release/cmn
	@mkdir -p $(OBJ_DIR)/release/liam
	@mkdir -p $(OBJ_DIR)/release/philemon
	@mkdir -p $(OBJ_DIR)/release/salome
	@mkdir -p $(OBJ_DIR)/release/shlink
	@mkdir -p $(OBJ_DIR)/release/shtasm
	@mkdir -p $(OBJ_DIR)/release/shtemu
	@mkdir -p $(OBJ_DIR)/release/syzygy
	@mkdir -p $(OUT_DIR)/debug
	@mkdir -p $(OUT_DIR)/release

.PHONY: dtest rtest atest debug all clean dirs

# ----------------------------------------------------------------------
# cmn

CMN_SRC = \
	src/cmn/align.cpp \
	src/cmn/ast.cpp \
	src/cmn/autoDump.cpp \
	src/cmn/binReader.cpp \
	src/cmn/binWriter.cpp \
	src/cmn/cmdline.cpp \
	src/cmn/commonLexor.cpp \
	src/cmn/commonParser.cpp \
	src/cmn/fmt.cpp \
	src/cmn/i64asm.cpp \
	src/cmn/intel64.cpp \
	src/cmn/lexor.cpp \
	src/cmn/loopIntrinsicDecomp.cpp \
	src/cmn/nameUtil.cpp \
	src/cmn/obj-fmt.cpp \
	src/cmn/out.cpp \
	src/cmn/pathUtil.cpp \
	src/cmn/symbolTable.cpp \
	src/cmn/target.cpp \
	src/cmn/textTable.cpp \
	src/cmn/throw.cpp \
	src/cmn/trace.cpp \
	src/cmn/type.cpp \
	src/cmn/typeVisitor.cpp \
	src/cmn/unique.cpp \
	src/cmn/userError.cpp \

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
# appr

APPR_SRC = \
	src/appr/instr.cpp \
	src/appr/main.cpp \
	src/appr/scriptWriter.cpp \
	src/appr/test.cpp \

APPR_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(APPR_SRC)))

$(OUT_DIR)/debug/appr.exe: $(APPR_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(APPR_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(APPR_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

APPR_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(APPR_SRC)))

$(OUT_DIR)/release/appr.exe: $(APPR_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(APPR_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(APPR_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# syzygy

SYZYGY_SRC = \
	src/araceli/consoleAppTarget.cpp \
	src/araceli/lexor.cpp \
	src/araceli/loader.cpp \
	src/araceli/metadata.cpp \
	src/araceli/objectBaser.cpp \
	src/araceli/projectBuilder.cpp \
	src/araceli/symbolTable.cpp \
	src/syzygy/codegen.cpp \
	src/syzygy/frontend.cpp \
	src/syzygy/genericUnlinker.cpp \
	src/syzygy/symbolTable.cpp \

SYZYGY_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(SYZYGY_SRC)))

$(OUT_DIR)/debug/syzygy.lib: $(SYZYGY_DEBUG_OBJ)
	$(info $< --> $@)
	@ar crs $@ $^

$(SYZYGY_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

SYZYGY_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(SYZYGY_SRC)))

$(OUT_DIR)/release/syzygy.lib: $(SYZYGY_RELEASE_OBJ)
	$(info $< --> $@)
	@ar crs $@ $^

$(SYZYGY_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# salome

SALOME_SRC = \
	src/salome/intfTransform.cpp \
	src/salome/main.cpp \
	src/salome/nodeFlagChecker.cpp \
	src/salome/symbolTable.cpp \

SALOME_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(SALOME_SRC)))

$(OUT_DIR)/debug/salome.exe: $(SALOME_DEBUG_OBJ) $(OUT_DIR)/debug/syzygy.lib $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SALOME_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lsyzygy -lcmn

$(SALOME_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

SALOME_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(SALOME_SRC)))

$(OUT_DIR)/release/salome.exe: $(SALOME_RELEASE_OBJ) $(OUT_DIR)/release/syzygy.lib $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SALOME_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lsyzygy -lcmn

$(SALOME_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# philemon

PHILEMON_SRC = \
	src/philemon/arrayDecomposition.cpp \
	src/philemon/frontend.cpp \
	src/philemon/genericClassInstantiator.cpp \
	src/philemon/main.cpp \
	src/philemon/stringDecomposition.cpp \
	src/philemon/symbolTable.cpp \

PHILEMON_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(PHILEMON_SRC)))

$(OUT_DIR)/debug/philemon.exe: $(PHILEMON_DEBUG_OBJ) $(OUT_DIR)/debug/syzygy.lib $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(PHILEMON_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lsyzygy -lcmn

$(PHILEMON_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

PHILEMON_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(PHILEMON_SRC)))

$(OUT_DIR)/release/philemon.exe: $(PHILEMON_RELEASE_OBJ) $(OUT_DIR)/release/syzygy.lib $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(PHILEMON_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lsyzygy -lcmn

$(PHILEMON_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# araceli

ARACELI_SRC = \
	src/araceli/abstractGenerator.cpp \
	src/araceli/accessChecker.cpp \
	src/araceli/batGen.cpp \
	src/araceli/classInfo.cpp \
	src/araceli/codegen.cpp \
	src/araceli/constHoister.cpp \
	src/araceli/ctorDtorGenerator.cpp \
	src/araceli/inheritImplementor.cpp \
	src/araceli/main.cpp \
	src/araceli/matryoshkaDecomp.cpp \
	src/araceli/methodMover.cpp \
	src/araceli/opOverloadDecomp.cpp \
	src/araceli/selfDecomposition.cpp \
	src/araceli/stackClassDecomposition.cpp \
	src/araceli/subprocess.cpp \
	src/araceli/vtableGenerator.cpp \

ARACELI_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(ARACELI_SRC)))

$(OUT_DIR)/debug/araceli.exe: $(ARACELI_DEBUG_OBJ) $(OUT_DIR)/debug/syzygy.lib $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(ARACELI_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lsyzygy -lcmn

$(ARACELI_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

ARACELI_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(ARACELI_SRC)))

$(OUT_DIR)/release/araceli.exe: $(ARACELI_RELEASE_OBJ) $(OUT_DIR)/release/syzygy.lib $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(ARACELI_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lsyzygy -lcmn

$(ARACELI_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# liam

LIAM_SRC = \
	src/liam/asmCodeGen.cpp \
	src/liam/astCodeGen.cpp \
	src/liam/availVarPass.cpp \
	src/liam/instrPrefs.cpp \
	src/liam/lexor.cpp \
	src/liam/lir.cpp \
	src/liam/lirXfrm.cpp \
	src/liam/loopTransforms.cpp \
	src/liam/main.cpp \
	src/liam/projectBuilder.cpp \
	src/liam/vTableInvokeDetection.cpp \
	src/liam/varAlloc.cpp \
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

# ----------------------------------------------------------------------
# shtasm

SHTASM_SRC = \
	src/shtasm/assembler.cpp \
	src/shtasm/frontend.cpp \
	src/shtasm/main.cpp \
	src/shtasm/processor.cpp \

SHTASM_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(SHTASM_SRC)))

$(OUT_DIR)/debug/shtasm.exe: $(SHTASM_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SHTASM_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(SHTASM_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

SHTASM_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(SHTASM_SRC)))

$(OUT_DIR)/release/shtasm.exe: $(SHTASM_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SHTASM_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(SHTASM_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# shlink

SHLINK_SRC = \
	src/shlink/formatter.cpp \
	src/shlink/iTarget.cpp \
	src/shlink/layout.cpp \
	src/shlink/main.cpp \
	src/shlink/objdir.cpp \
	src/shlink/shtankyAppTarget.cpp \

SHLINK_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(SHLINK_SRC)))

$(OUT_DIR)/debug/shlink.exe: $(SHLINK_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SHLINK_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(SHLINK_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

SHLINK_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(SHLINK_SRC)))

$(OUT_DIR)/release/shlink.exe: $(SHLINK_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SHLINK_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(SHLINK_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@

# ----------------------------------------------------------------------
# shtemu

SHTEMU_SRC = \
	src/shtemu/allocator.cpp \
	src/shtemu/loader.cpp \
	src/shtemu/main.cpp \
	src/shtemu/osCall.cpp \
	src/shtemu/servicePrint.cpp \

SHTEMU_DEBUG_OBJ = $(subst src,$(OBJ_DIR)/debug,$(patsubst %.cpp,%.o,$(SHTEMU_SRC)))

$(OUT_DIR)/debug/shtemu.exe: $(SHTEMU_DEBUG_OBJ) $(OUT_DIR)/debug/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SHTEMU_DEBUG_OBJ) $(DEBUG_LNK_FLAGS_POST) -Lbin/out/debug -lcmn

$(SHTEMU_DEBUG_OBJ): $(OBJ_DIR)/debug/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(DEBUG_CC_FLAGS) $< -o $@

SHTEMU_RELEASE_OBJ = $(subst src,$(OBJ_DIR)/release,$(patsubst %.cpp,%.o,$(SHTEMU_SRC)))

$(OUT_DIR)/release/shtemu.exe: $(SHTEMU_RELEASE_OBJ) $(OUT_DIR)/release/cmn.lib
	$(info $< --> $@)
	@$(LINK_CMD) -o $@ $(SHTEMU_RELEASE_OBJ) $(RELEASE_LNK_FLAGS_POST) -Lbin/out/release -lcmn

$(SHTEMU_RELEASE_OBJ): $(OBJ_DIR)/release/%.o: src/%.cpp
	$(info $< --> $@)
	@$(COMPILE_CMD) $(RELEASE_CC_FLAGS) $< -o $@
