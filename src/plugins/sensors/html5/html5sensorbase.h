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

#ifndef QHTML5SENSORBASE_H
#define QHTML5SENSORBASE_H

#include <qsensorbackend.h>
#include <qsensor.h>

#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QOrientationReading>

#include <QProximityReading>
#include <QLightSensor>


#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

class QHtml5SensorBase : public QSensorBackend
{
    Q_OBJECT
public:
    QHtml5SensorBase(QSensor *sensor);

    void start() override;
    void stop() override;

    QAccelerometer::AccelerationMode accelMode;
    static QHtml5SensorBase *get();
    void emitDeviceLightChanged(int value);
    void emitDeviceProximityChanged(bool isNear);


signals:
    void accelDataAvailable(const QAccelerometerReading &);
    void gyroDataAvailable(const QGyroscopeReading &);

    void proximityReadingAvailable(const QProximityReading &);
    void lightReadingAvailable(const QLightReading &);

private:
    QAccelerometerReading accelReading;

    QGyroscopeReading gyroReading;
    QOrientationReading orientationReading;

    QProximityReading proxyReading;
    QLightReading lightReading;

    static int deviceOrientation_cb(int eventType, const EmscriptenDeviceOrientationEvent *deviceOrientationEvent, void *userData);
    static int deviceMotion_cb(int eventType, const EmscriptenDeviceMotionEvent *deviceMotionEvent, void *userData);

    void updateAccelerationData( const EmscriptenDeviceMotionEvent *deviceMotionEvent);

private slots:
    void modeChanged(QAccelerometer::AccelerationMode accelerationMode);

};

#endif

