QT += core gui widgets network sql

TARGET = AviaMOS
TEMPLATE = app

# Источники
SOURCES += \
    main.cpp \
    loginwindow.cpp \
    mainwindow.cpp \
    clientapi.cpp \
    mytcpserver.cpp \
    databasesingleton.cpp \
    ticket.cpp

# Заголовки
HEADERS += \
    loginwindow.h \
    mainwindow.h \
    clientapi.h \
    mytcpserver.h \
    databasesingleton.h \
    ticket.h

# UI-формы
FORMS += \
    loginwindow.ui \
    mainwindow.ui

# Настройки для Windows
win32 {
    QMAKE_CXXFLAGS += -std=c++17
    DEFINES += QT_DEPRECATED_WARNINGS
}

# Путь к базе данных (относительный)
DEFINES += DATABASE_PATH=\"$$PWD/AviaMOS.db\"
