/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#pragma once

#include "window/namespace.h"

#include <DListView>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class MouseModel;
class MouseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MouseWidget(QWidget *parent = nullptr);
    void initSetting(const int settingIndex);
private:
    void init();
    void onItemClieck(const QModelIndex &index);
Q_SIGNALS:
    void showGeneralSetting();
    void showMouseSetting();
    void showTouchpadSetting();
    void showTrackPointSetting();
private:
    MouseModel *m_mouseModel;
    DTK_WIDGET_NAMESPACE::DListView *m_mouseListView;
    QVBoxLayout *m_contentLayout;
    QStandardItemModel *m_listviewModel;
};
}
}
