TARGET = bin/SagaCross.html

CXX := emcc

#TOTAL_MEMORY must be a multiple of 16MB

EMFLAGS += -s ALLOW_MEMORY_GROWTH=1 
EMFLAGS += -s TOTAL_MEMORY=268435456 
EMFLAGS += -s NO_EXIT_RUNTIME=1
EMFLAGS += -s DISABLE_EXCEPTION_CATCHING=1
EMFLAGS += --memory-init-file 0

CPPFLAGS += -std=c++14 -w -O2  
CPPFLAGS += $(EMFLAGS)

LIBS := -s USE_GLFW=3 -s FULL_ES2=1 
LIBS += $(EMFLAGS)
LIBS += -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' 
LIBS += -s DEMANGLE_SUPPORT=1 

VPATH += ../../source
VPATH += ../../source/Math
VPATH += ../../source/Timer
VPATH += ../../SagaCross

CPPFLAGS += -I../../source/
CPPFLAGS += -I../../source/Math/
CPPFLAGS += -I../../source/Timer/
CPPFLAGS += -I../../libraries/FMOD/inc/

COMMON_SRC := ../../SagaCross/SagaCross.cpp \
			  ../../source/BasePathExtraction.cpp \
			  ../../source/ShaderProgram.cpp \
			  ../../source/Core-pch.cpp \
			  ../../source/SCGLFWDisplay.cpp \
			  ../../source/SCSettings.cpp \
			  ../../source/SCSceneManager.cpp \
			  ../../source/SCEntity.cpp \
			  ../../source/SCDisplay.cpp \
			  ../../source/SCRenderScene.cpp \
			  ../../source/SCRenderer.cpp \
			  ../../source/SCDebugScene.cpp \
			  ../../source/SCAudio.cpp \
			  ../../source/SCOpenGLRenderer.cpp \
			  ../../source/SCVehicle.cpp \
			  ../../source/SCTank.cpp \
			  ../../source/SCNewGameScene.cpp \
			  ../../source/SCApplication.cpp \
			  ../../source/glTextureLoader.cpp \
			  ../../source/SCMenuScene.cpp \
			  ../../source/SCSparkles.cpp \
			  ../../source/SCSettingsScene.cpp \
			  ../../source/SCGLFWInput.cpp \
			  ../../source/glMatrix4x4.cpp \
			  ../../source/Math/xMath.cpp \
			  ../../source/Math/xVec3.cpp \
			  ../../source/Timer/GameTimer.cpp \
			  ../../source/Timer/AndroidTimer.cpp

COMMON_OBJS := $(patsubst %.cpp, bin/%.bc, $(notdir $(COMMON_SRC)))
COMMON_OBJS += ../../libraries/FMOD/lib/html5/fmod.bc 

all: copy $(TARGET)

copy: generate

	cp -a ../../resource/* bin/resource/

generate: clean

	@mkdir -p bin	
	@mkdir -p bin/resource	

clean:
	@rm -rf bin

$(TARGET) : $(COMMON_OBJS)
	$(CXX) $^ -o $@ $(LIBS) --preload-file bin/resource@resource

bin/%.bc: %.cpp
	$(CXX) $(CPPFLAGS) $< -o $@
