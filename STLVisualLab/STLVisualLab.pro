QT += core gui widgets
TARGET = STLVisualLab
TEMPLATE = app
CONFIG += c++17

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    VisualVector.cpp \
    VisualList.cpp \
    VisualStackQueue.cpp \
    VisualSort.cpp \
    VisualLowerBound.cpp \
    VisualReverse.cpp \
    VisualUnique.cpp \
    VisualMerge.cpp \
    SubQuizWidget.cpp \
    QuizDatabase.cpp \
    UserProfile.cpp \
    VisualRemove.cpp \
    VisualRotate.cpp \
    VisualPermutation.cpp \

HEADERS += \
    MainWindow.h \
    VisualBase.h \
    VisualVector.h \
    VisualList.h \
    VisualStackQueue.h \
    VisualSort.h \
    VisualLowerBound.h \
    VisualReverse.h \
    VisualUnique.h \
    VisualMerge.h \
    SubQuizWidget.h \
    QuizDatabase.h \
    UserProfile.h \
    CppHighlighter.h \
    VisualRemove.h \
    VisualRotate.h \
    VisualPermutation.h

RESOURCES += \
    resources.qrc \
