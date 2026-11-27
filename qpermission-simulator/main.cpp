/*
    SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>

#include <iostream>

using namespace Qt::Literals;

static void printUsage()
{
    std::cerr << "Usage: qpermission-simulator [--ask|--grant|--deny] <application>" << std::endl;
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    auto args = app.arguments();
    args.pop_front();
    if (args.empty()) {
        printUsage();
        return -1;
    }

    auto env = QProcessEnvironment::systemEnvironment();

    // find the permission simulation plugin
    const QString pluginPath = QCoreApplication::applicationDirPath() + "/../" KDE_INSTALL_LIBDIR "/qpermission-simulator/plugins"_L1;
    if (!QFileInfo::exists(pluginPath + "/permissions"_L1)) {
        qFatal() << "permission simulation plugin not found!";
        return -1;
    }
    QString searchPath = env.value(u"QT_PLUGIN_PATH"_s);
    if (searchPath.isEmpty()) {
        searchPath = pluginPath;
    } else {
        searchPath = pluginPath + QDir::listSeparator() + searchPath;
    }
    env.insert(u"QT_PLUGIN_PATH"_s, searchPath);

    // handle default permission policies
    if (args.front() == "--ask"_L1) {
        env.insert(u"QPERMISSION_SIMULATOR_DEFAULT_POLICY"_s, u"ask"_s);
        args.pop_front();
    } else if (args.front() == "--grant"_L1) {
        env.insert(u"QPERMISSION_SIMULATOR_DEFAULT_POLICY"_s, u"grant"_s);
        args.pop_front();
    } else if (args.front() == "--deny"_L1) {
        env.insert(u"QPERMISSION_SIMULATOR_DEFAULT_POLICY"_s, u"deny"_s);
        args.pop_front();
    }

    if (args.empty()) {
        printUsage();
        return -1;
    }

    QProcess proc;
    proc.setProcessEnvironment(env);
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    QObject::connect(&proc, &QProcess::finished, &app, [](int exitCode) { QCoreApplication::exit(exitCode); });
    proc.setProgram(args.front());
    args.pop_front();
    proc.setArguments(args);
    proc.start();

    return app.exec();
}
