/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "html5sensorbase.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <emscripten.h>

#include <QDebug>

using namespace emscripten;

void proximityCallback(bool isNear)
{
    qDebug() << Q_FUNC_INFO << isNear;
    QHtml5SensorBase::get()->emitDeviceProximityChanged(isNear);
}

void lightSensorCallback(int lightValue)
{
    qDebug() << Q_FUNC_INFO << lightValue;
    QHtml5SensorBase::get()->emitDeviceLightChanged(lightValue);
}

EMSCRIPTEN_BINDINGS(sensor_module) {
    function("proximityCallback", &proximityCallback);
    function("lightSensorCallback", &lightSensorCallback);
}


static QHtml5SensorBase *globalQHtml5SensorBase;

QHtml5SensorBase *get() { return globalQHtml5SensorBase; }

QHtml5SensorBase::QHtml5SensorBase(QSensor *sensor)
    : QSensorBackend(sensor),
      accelMode(QAccelerometer::Combined)
{
    if (sensor->type() == QOrientationSensor::type) {
        emscripten_set_deviceorientation_callback((void *)this, 1, deviceOrientation_cb);
    }

    if (sensor->type() == QProximitySensor::type) {
        EM_ASM(
          addEventListener('userproximity', function(event) {
               proximityCallback(event.near);
          });
        );
    }

    if (sensor->type() == QLightSensor::type) {
        EM_ASM(
           addEventListener('devicelight', function(event) {
                lightSensorCallback(event.value);
           });
        );
    }

    if (sensor->type() == QAccelerometer::type) {
        emscripten_set_devicemotion_callback((void *)this, 1, deviceMotion_cb);
        QAccelerometer *accelSensor = static_cast<QAccelerometer *>(sensor);
        connect(accelSensor, &QAccelerometer::accelerationModeChanged, this, &QHtml5SensorBase::modeChanged);
    }
}

void QHtml5SensorBase::start()
{
}

void QHtml5SensorBase::stop()
{
}

int QHtml5SensorBase::deviceOrientation_cb(int eventType, const EmscriptenDeviceOrientationEvent *deviceOrientationEvent, void *userData)
{
    QHtml5SensorBase *sensorBase = reinterpret_cast<QHtml5SensorBase *>(userData);
//    switch (deviceOrientationEvent->orientationIndex) {
//    case EMSCRIPTEN_ORIENTATION_PORTRAIT_PRIMARY:
//        break;
//    case EMSCRIPTEN_ORIENTATION_PORTRAIT_SECONDARY:
//        break;
//    case EMSCRIPTEN_ORIENTATION_LANDSCAPE_PRIMARY:
//        break;
//    case EMSCRIPTEN_ORIENTATION_LANDSCAPE_SECONDARY:
//        break;
//    };
    // deviceOrientationEvent->orientationAngle
    return 0;
}

int QHtml5SensorBase::deviceMotion_cb(int eventType, const EmscriptenDeviceMotionEvent *deviceMotionEvent, void *userData)
{
    QHtml5SensorBase *sensorBase = reinterpret_cast<QHtml5SensorBase *>(userData);
    if (sensorBase)
        sensorBase->updateAccelerationData(deviceMotionEvent);
    return 0;
}

void QHtml5SensorBase::updateAccelerationData(const EmscriptenDeviceMotionEvent *deviceMotionEvent)
{
    if (sensor()->type() == QAccelerometer::type) {
        accelReading.setTimestamp(deviceMotionEvent->timestamp);
        switch (accelMode) {
        case QAccelerometer::Combined:
            accelReading.setX(deviceMotionEvent->accelerationIncludingGravityX);
            accelReading.setY(deviceMotionEvent->accelerationIncludingGravityY);
            accelReading.setZ(deviceMotionEvent->accelerationIncludingGravityZ);
            break;
        case QAccelerometer::Gravity:
            accelReading.setX(deviceMotionEvent->accelerationIncludingGravityX - deviceMotionEvent->accelerationX);
            accelReading.setY(deviceMotionEvent->accelerationIncludingGravityY - deviceMotionEvent->accelerationY);
            accelReading.setZ(deviceMotionEvent->accelerationIncludingGravityZ - deviceMotionEvent->accelerationZ);
            break;
        case QAccelerometer::User:
            accelReading.setX(deviceMotionEvent->accelerationX);
            accelReading.setY(deviceMotionEvent->accelerationY);
            accelReading.setZ(deviceMotionEvent->accelerationZ);
            break;
        };
        accelDataAvailable(accelReading);
    }

    if (sensor()->type() == QGyroscope::type) {
        gyroReading.setX(deviceMotionEvent->rotationRateAlpha); //GyroscopeReading ?
        gyroReading.setY(deviceMotionEvent->rotationRateBeta);
        gyroReading.setZ(deviceMotionEvent->rotationRateGamma);
        gyroDataAvailable(gyroReading);
    }
}

void QHtml5SensorBase::modeChanged(QAccelerometer::AccelerationMode accelerationMode)
{
   accelMode = accelerationMode;
}

void QHtml5SensorBase::emitDeviceLightChanged(int value)
{
    lightReading.setLux(value);
    lightReadingAvailable(lightReading);
}

void QHtml5SensorBase::emitDeviceProximityChanged(bool isNear)
{
    proxyReading.setClose(isNear);
    proximityReadingAvailable(proxyReading);
}

