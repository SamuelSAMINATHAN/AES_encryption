TEMPLATE = app
CONFIG += console c11
TARGET = encryptor-cli

SOURCES += main_cli.c \
           ../src/secure_aes_gcm.c
HEADERS += ../include/secure_aes_gcm.h

INCLUDEPATH += ../include \
               /opt/homebrew/opt/openssl@3/include

LIBS += -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
