TEMPLATE = app
CONFIG += c++17
QT += widgets

TARGET = encryptor-gui

SOURCES += main_gui.cpp \
           ../src/secure_aes_gcm.c

HEADERS += ../include/secure_aes_gcm.h

INCLUDEPATH += ../include \
               /opt/homebrew/opt/openssl@3/include

LIBS += -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
