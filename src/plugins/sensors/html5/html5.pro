TARGET = qtsensors_html5
QT = sensors core

HEADERS += html5sensorbase.h\
           html5accelerometer.h \
           html5alssensor.h \
           html5proximitysensor.h

SOURCES += html5sensorbase.cpp\
           html5accelerometer.cpp \
           html5alssensor.cpp \
           html5proximitysensor.cpp \
           main.cpp

OTHER_FILES = plugin.json


PLUGIN_TYPE = sensors
PLUGIN_CLASS_NAME = Html5SensorPlugin
load(qt_plugin)
