/*
    SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "qpermissionsimulatorplugin.h"

#include <QDebug>
#include <QGuiApplication>
#include <QMessageBox>
#include <QWindow>

using namespace Qt::Literals;

QPermissionSimulatorPlugin::QPermissionSimulatorPlugin() = default;
QPermissionSimulatorPlugin::~QPermissionSimulatorPlugin() = default;

Qt::PermissionStatus QPermissionSimulatorPlugin::checkPermission(const QPermission &permission)
{
    const auto policy = qEnvironmentVariable("QPERMISSION_SIMULATOR_DEFAULT_POLICY", u"ask"_s);
    if (policy == "grant"_L1) {
        return Qt::PermissionStatus::Granted;
    }
    if (policy == "deny"_L1) {
        return Qt::PermissionStatus::Denied;
    }

    if (permission.type().id() == qMetaTypeId<QLocationPermission>()) {
        return m_locationPermissions.value(permission.value<QLocationPermission>().value().accuracy(), Qt::PermissionStatus::Undetermined);
    }
    if (permission.type().id() == qMetaTypeId<QCameraPermission>()) {
        return m_cameraPermission;
    }
    if (permission.type().id() == qMetaTypeId<QMicrophonePermission>()) {
        return m_microphonePermission;
    }
    if (permission.type().id() == qMetaTypeId<QBluetoothPermission>()) {
        return m_bluetoothPermission;
    }
    if (permission.type().id() == qMetaTypeId<QCalendarPermission>()) {
        const auto accessMode = permission.value<QCalendarPermission>().value().accessMode();
        const auto roPerm = m_caleandarPermissions.value(QCalendarPermission::ReadOnly, Qt::PermissionStatus::Undetermined);
        const auto rwPerm = m_caleandarPermissions.value(QCalendarPermission::ReadWrite, Qt::PermissionStatus::Undetermined);
        if (accessMode == QCalendarPermission::ReadWrite) {
            return rwPerm;
        }
        if (accessMode == QCalendarPermission::ReadOnly && rwPerm == Qt::PermissionStatus::Granted) {
            return rwPerm;
        }
        return roPerm;
    }
    if (permission.type().id() == qMetaTypeId<QContactsPermission>()) {
        const auto accessMode = permission.value<QContactsPermission>().value().accessMode();
        const auto roPerm = m_contactPermissions.value(QContactsPermission::ReadOnly, Qt::PermissionStatus::Undetermined);
        const auto rwPerm = m_contactPermissions.value(QContactsPermission::ReadWrite, Qt::PermissionStatus::Undetermined);
        if (accessMode == QContactsPermission::ReadWrite) {
            return rwPerm;
        }
        if (accessMode == QContactsPermission::ReadOnly && rwPerm == Qt::PermissionStatus::Granted) {
            return rwPerm;
        }
        return roPerm;
    }

    return Qt::PermissionStatus::Undetermined;
}

void QPermissionSimulatorPlugin::ask(const QString &title, const QString &text, Qt::PermissionStatus &perm, const QPermissions::Private::PermissionCallback &callback)
{
    QWindow *parent = nullptr;
    if (!QGuiApplication::topLevelWindows().isEmpty()) {
        parent = QGuiApplication::topLevelWindows().front();
    }
    auto box = new QMessageBox(QMessageBox::Question, title, text, QMessageBox::Yes|QMessageBox::No);
    box->winId();
    box->windowHandle()->setTransientParent(parent);
    connect(box, &QMessageBox::finished, this, [&perm, callback](int result) {
        perm = result == QMessageBox::Yes ? Qt::PermissionStatus::Granted : Qt::PermissionStatus::Denied;
        callback(perm);
    });
    box->show();
}

void QPermissionSimulatorPlugin::requestPermission(const QPermission &permission, const QPermissions::Private::PermissionCallback &callback)
{
    if (permission.type().id() == qMetaTypeId<QLocationPermission>()) {
        const auto accuracy = permission.value<QLocationPermission>().value().accuracy();
        ask(u"Location Permission"_s, accuracy == QLocationPermission::Precise ? u"Grant precise location access?"_s : u"Grant approximate location access"_s, m_locationPermissions[accuracy], callback);
    }

    if (permission.type().id() == qMetaTypeId<QCameraPermission>()) {
        ask(u"Camera Permission"_s, u"Grant camera access?"_s, m_cameraPermission, callback);
    }

    if (permission.type().id() == qMetaTypeId<QMicrophonePermission>()) {
        ask(u"Microphone Permission"_s, u"Grant microphone access?"_s, m_microphonePermission, callback);
    }

    if (permission.type().id() == qMetaTypeId<QBluetoothPermission>()) {
        ask(u"Bluetooth Permission"_s, u"Grant bluetooth access?"_s, m_bluetoothPermission, callback);
    }

    if (permission.type().id() == qMetaTypeId<QCalendarPermission>()) {
        const auto accessMode = permission.value<QCalendarPermission>().value().accessMode();
        ask(u"Calendar Permission"_s, accessMode == QCalendarPermission::ReadOnly ? u"Grant read-only calendar access?"_s : u"Grant read-write calendar access?"_s, m_caleandarPermissions[accessMode], callback);
    }

    if (permission.type().id() == qMetaTypeId<QContactsPermission>()) {
        const auto accessMode = permission.value<QContactsPermission>().value().accessMode();
        ask(u"Contact Permission"_s, accessMode == QContactsPermission::ReadOnly ? u"Grant read-only contact access?"_s : u"Grant read-write contact access?"_s, m_contactPermissions[accessMode], callback);
    }

    callback(Qt::PermissionStatus::Undetermined);
};
