/*
    SPDX-FileCopyrightText: 2008 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KPARTLOADER_H
#define KPARTLOADER_H

// KF
#include <KParts/MainWindow>
#include <KParts/ReadOnlyPart>

class KPartLoaderWindow : public KParts::MainWindow
{
  Q_OBJECT
public:
  explicit KPartLoaderWindow(const QString& partLib);
  ~KPartLoaderWindow() override;

private Q_SLOTS:
  void aboutKPart();

private:
  KParts::ReadOnlyPart *m_part;
};

#endif
