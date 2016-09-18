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
    CameraLogic.cpp \
    Game.cpp \
    Global.cpp \
    Fish.cpp \
    Barrier.cpp \
    Environment.cpp \
    Weed.cpp

HEADERS += \
    CameraLogic.h \
    Global.h \
    Urho3DAll.h \
    Barrier.h \
    Environment.h \
    Fish.h \
    Weed.h

OTHER_FILES += \
