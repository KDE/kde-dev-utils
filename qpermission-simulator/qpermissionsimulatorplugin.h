/*
    SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <private/qpermissions_p.h>

#include <QMap>

class QPermissionSimulatorPlugin : public QPermissionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.QPermissionPluginInterface.6.5" FILE "plugin.json")

public:
    explicit QPermissionSimulatorPlugin();
    ~QPermissionSimulatorPlugin();

    Qt::PermissionStatus checkPermission(const QPermission &permission) override;
    void requestPermission(const QPermission &permission, const QPermissions::Private::PermissionCallback &callback) override;

private:
    void ask(const QString &title, const QString &text, Qt::PermissionStatus &perm, const QPermissions::Private::PermissionCallback &callback);

    QMap<QLocationPermission::Accuracy, Qt::PermissionStatus> m_locationPermissions;
    QMap<QCalendarPermission::AccessMode, Qt::PermissionStatus> m_caleandarPermissions;
    QMap<QContactsPermission::AccessMode, Qt::PermissionStatus> m_contactPermissions;
    Qt::PermissionStatus m_cameraPermission = Qt::PermissionStatus::Undetermined;
    Qt::PermissionStatus m_microphonePermission = Qt::PermissionStatus::Undetermined;
    Qt::PermissionStatus m_bluetoothPermission = Qt::PermissionStatus::Undetermined;
};
