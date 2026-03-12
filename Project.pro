QT       += core gui
#QT       += texttospeech # TTS模块
QT       += sql # 数据库模块
QT       += multimedia multimediawidgets # 多媒体模块
QT       += network #网络模块
QT       += widgets
QT       += printsupport
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 # 使用 C++ 2011 年的标准

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# MATERIAL-SDK UI库
LIBS += $$PWD/SDK/MaterialSDK/staticLib/libcomponents.a
INCLUDEPATH += $$PWD/SDK/MaterialSDK/components
PRE_TARGETDEPS += $$PWD/SDK/MaterialSDK/staticLib/libcomponents.a


# 编译源文件
SOURCES += \
    CircleAvatar.cpp \
    DatabaseManager.cpp \
    main.cpp \
    mainwindow.cpp \
    neumorphic.cpp \
    passwordtogglehelper.cpp \
    qcustomplot.cpp \
    regdialog.cpp \
    second.cpp \
    sqlit3test.cpp \
    system_main_page.cpp

# 编译头文件
HEADERS += \
    CircleAvatar.h \
    DatabaseManager.h \
    mainwindow.h \
    neumorphic.h \
    passwordtogglehelper.h \
    qcustomplot.h \
    regdialog.h \
    second.h \
    sqlit3test.h \
    system_main_page.h

# 编译UI文件
FORMS += \
    mainwindow.ui \
    regdialog.ui \
    second.ui \
    sqlit3test.ui \
    system_main_page.ui

TRANSLATIONS += \
    Project_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc \
    resource/resource.qrc

DISTFILES += \
    resource/font/字魂布丁体(商用需授权).ttf \
    resource/img/bilibili.png \
    resource/img/browser.png \
    resource/img/calendar.png \
    resource/img/directory.png \
    resource/img/phone.png \
    resource/img/record.png \
    resource/img/text.png \
    resource/img/twitter.png \
    resource/img/wechat.png
