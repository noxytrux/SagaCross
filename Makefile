TARGET = bin/SagaCross

# Flags
CPPFLAGS += -std=c++14 -w -O2 

LIBS :=
LDFLAGS :=

#bind paths from external souces
VPATH := source:source/Math:source/Timer:SagaCross

# libs finding and binding
UNAME_S := $(shell uname -s)
GLFW3 := $(shell pkg-config --libs glfw3)

ifeq ($(UNAME_S),Darwin)
	LIBS := $(GLFW3) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib -Llibraries/FMOD/lib/macOS -lfmod -lfmodL
	CPPFLAGS += -I/usr/local/include
	LDFLAGS:= -rpath @executable_path/
else
	LIBS := $(GLFW3) -lGL -lm -lGLU -lGLEW -Llibraries/FMOD/lib/Linux -lfmod -lfmodL
	LDFLAGS:= -Wl,-rpath=.
endif

#additional includes
CPPFLAGS += -I./source/
CPPFLAGS += -I./source/Math/
CPPFLAGS += -I./source/Timer/
CPPFLAGS += -I./libraries/FMOD/inc/

#source files
COMMON_SRC := SagaCross/SagaCross.cpp \
			  source/BasePathExtraction.cpp \
			  source/ShaderProgram.cpp \
			  source/Core-pch.cpp \
			  source/SCGLFWDisplay.cpp \
			  source/SCSettings.cpp \
			  source/SCSceneManager.cpp \
			  source/SCEntity.cpp \
			  source/SCDisplay.cpp \
			  source/SCRenderScene.cpp \
			  source/SCRenderer.cpp \
			  source/SCDebugScene.cpp \
			  source/SCAudio.cpp \
			  source/SCOpenGLRenderer.cpp \
			  source/SCVehicle.cpp \
			  source/SCTank.cpp \
			  source/SCNewGameScene.cpp \
			  source/SCApplication.cpp \
			  source/glTextureLoader.cpp \
			  source/SCMenuScene.cpp \
			  source/SCSparkles.cpp \
			  source/SCSettingsScene.cpp \
			  source/SCGLFWInput.cpp \
			  source/glMatrix4x4.cpp \
			  source/Math/xMath.cpp \
			  source/Math/xVec3.cpp \
			  source/Timer/GameTimer.cpp 

ifeq ($(UNAME_S),Darwin)

COMMON_SRC += source/Timer/DarwinTimer.cpp

else

COMMON_SRC += source/Timer/AndroidTimer.cpp

endif	  

#generate bin/*.o paths from files
COMMON_OBJS := $(patsubst %.cpp, bin/%.o, $(notdir $(COMMON_SRC)))

#build
all: copy $(TARGET)

#copy files
copy: generate
	

ifeq ($(UNAME_S),Darwin)
	
	cp -a libraries/FMOD/lib/macOS/libfmod.dylib ./
	cp -a libraries/FMOD/lib/macOS/libfmodL.dylib ./
	
else 

	cp -a libraries/FMOD/lib/Linux/* ./

endif
	
#make bin 
generate: clean
	@mkdir -p bin	

#clean before build
clean:
	@rm -rf bin

#for each *.o file link with executable
$(TARGET) : $(COMMON_OBJS)
	@echo linking $@
	$(CXX) $^ -o $@ $(LIBS) $(LDFLAGS)
	mv $(TARGET) ./Game

#for each *.o file compile coresponding *.cpp file and put to bin/*.o
bin/%.o: %.cpp
	@echo compiling $< into $@
	$(CXX) $(CPPFLAGS) -c $< -o $@
