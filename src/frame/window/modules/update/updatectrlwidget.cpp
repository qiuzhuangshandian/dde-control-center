/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "updatectrlwidget.h"
#include "modules/update/updateitem.h"
#include "widgets/translucentframe.h"
#include "modules/update/updatemodel.h"
#include "loadingitem.h"
#include "widgets/labels/normallabel.h"
#include "widgets/settingsgroup.h"
#include "modules/update/summaryitem.h"
#include "modules/update/downloadprogressbar.h"
#include "modules/update/resultitem.h"
#include "widgets/labels/tipslabel.h"

#include <types/appupdateinfolist.h>
#include <QVBoxLayout>
#include <QSettings>

#define UpgradeWarningSize 500

using namespace dcc;
using namespace dcc::update;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateCtrlWidget::UpdateCtrlWidget(UpdateModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_model(nullptr)
    , m_status(UpdatesStatus::Updated)
    , m_checkUpdateItem(new LoadingItem)
    , m_resultItem(new ResultItem)
    , m_progress(new DownloadProgressBar)
    , m_summaryGroup(new SettingsGroup)
    , m_upgradeWarningGroup(new SettingsGroup)
    , m_summary(new SummaryItem)
    , m_upgradeWarning(new SummaryItem)
    , m_powerTip(new TipsLabel)
    , m_reminderTip(new TipsLabel)
    , m_noNetworkTip(new TipsLabel)
    , m_qsettings(new QSettings(this))
    , m_bRecoverBackingUp(false)
    , m_bRecoverConfigValid(false)
    , m_bRecoverRestoring(false)
{
    setTitle(tr("Update"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //~ contents_path /update/Update
    m_reminderTip->setText(tr("Restart the computer to use the system and the applications properly"));
    //~ contents_path /update/Update
    m_noNetworkTip->setText(tr("Network disconnected, please retry after connected"));

    m_progress->setVisible(false);

    m_summaryGroup->setVisible(false);

    m_powerTip->setWordWrap(true);
    m_powerTip->setAlignment(Qt::AlignHCenter);
    m_powerTip->setVisible(false);

    m_reminderTip->setWordWrap(true);
    m_reminderTip->setAlignment(Qt::AlignHCenter);
    m_reminderTip->setVisible(false);

    m_noNetworkTip->setWordWrap(true);
    m_noNetworkTip->setAlignment(Qt::AlignHCenter);
    m_noNetworkTip->setVisible(false);

    //~ contents_path /update/Update
    m_upgradeWarning->setTitle(tr("This update may take a long time, please do not shut down or reboot during the process"));
    m_upgradeWarning->setContentsMargins(20, 0, 20, 0);
    m_upgradeWarningGroup->setVisible(false);
    m_upgradeWarningGroup->appendItem(m_upgradeWarning);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->addStretch();
    layout->addWidget(m_resultItem);
    layout->addWidget(m_checkUpdateItem);
    layout->addWidget(m_upgradeWarningGroup);
    layout->addWidget(m_powerTip);
    layout->addWidget(m_reminderTip);
    layout->addWidget(m_noNetworkTip);
    layout->addWidget(m_summary);
    layout->addWidget(m_progress);
    layout->addWidget(m_summaryGroup);
    layout->addStretch();

    TranslucentFrame *widget = new TranslucentFrame();
    widget->setLayout(layout);
    setContent(widget);

    setModel(model);

    connect(m_progress, &DownloadProgressBar::clicked, this, &UpdateCtrlWidget::onProgressBarClicked);
}

UpdateCtrlWidget::~UpdateCtrlWidget()
{

}

void UpdateCtrlWidget::loadAppList(const QList<AppUpdateInfo> &infos)
{
//    qDebug() << infos.count();

    QLayoutItem *item;
    while ((item = m_summaryGroup->layout()->takeAt(0)) != nullptr) {
        item->widget()->deleteLater();
        delete item;
    }

    for (const AppUpdateInfo &info : infos) {
        UpdateItem *item = new UpdateItem();
        item->setAppInfo(info);

        m_summaryGroup->appendItem(item);
    }
}

void UpdateCtrlWidget::onProgressBarClicked()
{
    switch (m_status) {
    case UpdatesStatus::UpdatesAvailable:
        Q_EMIT requestDownloadUpdates();
        break;
    case UpdatesStatus::Downloading:
        Q_EMIT requestPauseDownload();
        break;
    case UpdatesStatus::DownloadPaused:
        Q_EMIT requestResumeDownload();
        break;
    case UpdatesStatus::Downloaded:
        Q_EMIT requestInstallUpdates();
        break;
    default:
        qWarning() << "unhandled status " << m_status;
        break;
    }
}

void UpdateCtrlWidget::setStatus(const UpdatesStatus &status)
{
    m_status = status;

    Q_EMIT notifyUpdateState(m_status);

    m_powerTip->setVisible(false);
    m_noNetworkTip->setVisible(false);
    m_resultItem->setVisible(false);
    m_progress->setVisible(false);
    m_summaryGroup->setVisible(false);
    m_upgradeWarningGroup->setVisible(false);
    m_reminderTip->setVisible(false);
    m_checkUpdateItem->setVisible(false);
    m_checkUpdateItem->setVisible(false);
    m_checkUpdateItem->setProgressBarVisible(false);
    m_checkUpdateItem->setImageAndTextVisible(false);
    m_summary->setVisible(false);

    switch (status) {
    case UpdatesStatus::Checking:
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setProgressBarVisible(true);
        //~ contents_path /update/Update
        m_checkUpdateItem->setMessage(tr("Checking for updates, please wait..."));
        m_checkUpdateItem->setImageOrTextVisible(false);
        break;
    case UpdatesStatus::UpdatesAvailable:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_summary->setVisible(true);
        //~ contents_path /update/Update
        m_progress->setMessage(tr("Download and install updates"));
        setDownloadInfo(m_model->downloadInfo());
        m_progress->setValue(100);
        setLowBattery(m_model->lowBattery());
        break;
    case UpdatesStatus::Downloading:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_summary->setVisible(true);
        m_progress->setValue(m_progress->value());
        //~ contents_path /update/Update
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(m_progress->value()));
        break;
    case UpdatesStatus::DownloadPaused:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_summary->setVisible(true);
        //~ contents_path /update/Update
        m_progress->setMessage(tr("%1% downloaded (Click to continue)").arg(m_progress->value()));
        break;
    case UpdatesStatus::Downloaded:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_summary->setVisible(true);
        m_progress->setValue(m_progress->maximum());
        //~ contents_path /update/Update
        m_progress->setMessage(tr("Install updates"));
        setDownloadInfo(m_model->downloadInfo());
        setLowBattery(m_model->lowBattery());
        break;
    case UpdatesStatus::Updated:
        m_checkUpdateItem->setVisible(true);
        m_checkUpdateItem->setVisible(true);
        //~ contents_path /update/Update
        m_checkUpdateItem->setMessage(tr("Your system is up to date"));
        m_checkUpdateItem->setImageOrTextVisible(true);
        m_checkUpdateItem->setSystemVersion(m_systemVersion);
        break;
    case UpdatesStatus::Installing:
        m_progress->setVisible(true);
        m_summaryGroup->setVisible(true);
        m_summary->setVisible(true);
        //~ contents_path /update/Update
        m_progress->setMessage(tr("Updating, please wait..."));
        break;
    case UpdatesStatus::UpdateSucceeded:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(true);
        m_reminderTip->setVisible(true);
        break;
    case UpdatesStatus::UpdateFailed:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(false);
        break;
    case UpdatesStatus::NeedRestart:
        m_checkUpdateItem->setVisible(true);
        //~ contents_path /update/Update
        m_checkUpdateItem->setMessage(tr("The newest system installed, restart to take effect"));
        break;
    case UpdatesStatus::NoNetwork:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(false);
        m_noNetworkTip->setVisible(true);
        break;
    case UpdatesStatus::NoSpace:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(false);
        //~ contents_path /update/Update
        m_resultItem->setMessage(tr("Update failed: insufficient disk space"));
        break;
    case UpdatesStatus::DeependenciesBrokenError:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(false);
        //~ contents_path /update/Update
        m_resultItem->setMessage(tr("Dependency error, failed to detect the updates"));
        break;
    case UpdatesStatus::RecoveryBackupFailed:
        m_resultItem->setVisible(true);
        m_resultItem->setSuccess(false);
        //~ contents_path /update/Update
        m_resultItem->setMessage(tr("System backup failed"));
        break;
    default:
        qWarning() << "unknown status!!!";
        break;
    }
}

void UpdateCtrlWidget::setDownloadInfo(DownloadInfo *downloadInfo)
{
    if (!downloadInfo)
        return;

    const QList<AppUpdateInfo> &apps = downloadInfo->appInfos();
    const qlonglong downloadSize = downloadInfo->downloadSize();

    int appCount = apps.length();
    for (const AppUpdateInfo &info : apps) {
        if (info.m_packageId == "dde") {
            appCount--;
        }
    }

    //~ contents_path /update/Update
    m_summary->setTitle(tr("%n application update(s) available", "", appCount));

    for (const AppUpdateInfo &info : apps) {
        if (info.m_packageId == "dde") {
            if (!appCount) {
                //~ contents_path /update/Update
                m_summary->setTitle(tr("New system edition available"));
            } else {
                //~ contents_path /update/Update
                m_summary->setTitle(tr("New system edition and %n application update(s) available", "", appCount));
            }
            break;
        }
    }

    if (!downloadSize) {
        //~ contents_path /update/Update
        m_summary->setDetails(tr("Downloaded"));
    } else {
        //~ contents_path /update/Update
        m_summary->setDetails(QString(tr("Size: %1").arg(formatCap(downloadSize))));

        //~ contents_path /update/Update
        if ((downloadSize / 1024) / 1024 >= m_qsettings->value("upgrade_waring_size", UpgradeWarningSize).toInt())
            m_upgradeWarningGroup->setVisible(true);
    }

    loadAppList(apps);
}

void UpdateCtrlWidget::setProgressValue(const double value)
{
    m_progress->setValue(value * 100);

    if (m_status == UpdatesStatus::Downloading) {
        //~ contents_path /update/Update
        m_progress->setMessage(tr("%1% downloaded (Click to pause)").arg(qFloor(value * 100)));
    }
}

void UpdateCtrlWidget::setLowBattery(const bool &lowBattery)
{
    if (m_status == UpdatesStatus::Downloaded || m_status == UpdatesStatus::UpdatesAvailable) {
        if (lowBattery) {
            //~ contents_path /update/Update
            m_powerTip->setText(tr("Your battery is lower than 50%, please plug in to continue"));
        } else {
            //~ contents_path /update/Update
            m_powerTip->setText(tr("Please ensure sufficient power to restart, and don't power off or unplug your machine"));
        }

        m_progress->setDisabled(lowBattery);
        m_powerTip->setVisible(lowBattery);
    }
}

void UpdateCtrlWidget::setUpdateProgress(const double value)
{
    m_checkUpdateItem->setProgressValue(value * 100);
}

void UpdateCtrlWidget::setRecoverBackingUp(const bool value)
{
    if (m_bRecoverBackingUp != value) {
        m_bRecoverBackingUp = value;
    }
}

void UpdateCtrlWidget::setRecoverConfigValid(const bool value)
{
    if (m_bRecoverConfigValid != value) {
        m_bRecoverConfigValid = value;
    }
}

void UpdateCtrlWidget::setRecoverRestoring(const bool value)
{
    if (m_bRecoverRestoring != value) {
        m_bRecoverRestoring = value;
    }
}

void UpdateCtrlWidget::setModel(UpdateModel *model)
{
    m_model = model;

    connect(m_model, &UpdateModel::statusChanged, this, &UpdateCtrlWidget::setStatus);
    connect(m_model, &UpdateModel::lowBatteryChanged, this, &UpdateCtrlWidget::setLowBattery);
    connect(m_model, &UpdateModel::downloadInfoChanged, this, &UpdateCtrlWidget::setDownloadInfo);
    connect(m_model, &UpdateModel::upgradeProgressChanged, this, &UpdateCtrlWidget::setProgressValue);
    connect(m_model, &UpdateModel::updateProgressChanged, this, &UpdateCtrlWidget::setUpdateProgress);
    connect(m_model, &UpdateModel::recoverBackingUpChanged, this, &UpdateCtrlWidget::setRecoverBackingUp);
    connect(m_model, &UpdateModel::recoverConfigValidChanged, this, &UpdateCtrlWidget::setRecoverConfigValid);
    connect(m_model, &UpdateModel::recoverRestoringChanged, this, &UpdateCtrlWidget::setRecoverRestoring);


    setUpdateProgress(m_model->updateProgress());
    setProgressValue(m_model->upgradeProgress());
    setStatus(m_model->status());
    setLowBattery(m_model->lowBattery());
    setDownloadInfo(m_model->downloadInfo());

    setDownloadInfo(m_model->downloadInfo());
}

void UpdateCtrlWidget::setSystemVersion(QString version)
{
    if (m_systemVersion != version) {
        m_systemVersion = version;
    }
}
