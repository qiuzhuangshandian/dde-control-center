/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "loginedin.h"
#include "downloadurl.h"
#include "avatarwidget.h"
#include "modules/unionid/unionidmodel.h"

#include <QDir>
#include <QLabel>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;

LoginedIn::LoginedIn(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
    , m_downloader(nullptr)
    , m_avatarPath(QString("%1/.cache/deepin/dde-control-center/sync").arg(getenv("HOME")))
    , m_avatar(new AvatarWidget)
    , m_username(new QLabel)
    , m_email(new QLabel)
{
    m_avatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_avatar->setFixedSize(QSize(100, 100));
    m_username->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

LoginedIn::~LoginedIn()
{
    if (m_downloader != nullptr)
        m_downloader->deleteLater();
}

void LoginedIn::onUserInfoChanged(const QVariantMap &infos)
{
    const bool isLogind = !infos["Username"].toString().isEmpty();
    const QString region = infos["Region"].toString();
    QString clasName = metaObject()->className();

    if (!isLogind)
        return;
    if (((clasName == "dccV20::sync::LogoutPage") && (region == "CN"))
            || ((clasName == "dccV20::sync::IndexPage") && (region != "CN")))
        return;

    m_username->setText(m_model->userDisplayName());
    m_email->setText(m_model->userEmail());
    QString avatarPath = QString("%1%2").arg(m_avatarPath).arg(m_model->userDisplayName());
    QDir dir;
    dir.mkpath(avatarPath);
    QString profile_image = infos.value("ProfileImage").toString();
    qDebug() << " ProfileImage = " << profile_image << ", avatarPath " << avatarPath;
    if (profile_image.isEmpty())
        return;

    if (m_downloader == nullptr)
        m_downloader = new DownloadUrl;

    connect(m_downloader, &DownloadUrl::fileDownloaded, this, [this](const QString & fileName) {
        qDebug() << "downloaded filename = " << fileName;
        m_avatar->setAvatarPath(fileName);
    });

    m_downloader->downloadFileFromURL(profile_image, avatarPath);
}

void LoginedIn::setModel(dcc::unionid::UnionidModel *model)
{
    m_model = model;
}
