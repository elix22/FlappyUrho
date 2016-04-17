LIBS += ../FlappyUrho/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += \
    ../FlappyUrho/Urho3D/include \
    ../FlappyUrho/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
BarrierLogic.cpp \
CameraLogic.cpp \
EnvironmentLogic.cpp \
Game.cpp \
Global.cpp \
UfoLogic.cpp

HEADERS += \
BarrierLogic.h \
CameraLogic.h \
EnvironmentLogic.h \
Global.h \
UfoLogic.h \
Urho3DAll.h

OTHER_FILES += \
