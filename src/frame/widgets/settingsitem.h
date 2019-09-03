/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef SETTINGSITEM_H
#define SETTINGSITEM_H

#include <DBackgroundGroup>

#include <QFrame>

namespace dcc {
namespace widgets {

class SettingsItem : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool isHead READ isHead DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(bool isTail READ isTail DESIGNABLE true SCRIPTABLE true)
    Q_PROPERTY(bool isErr READ isErr DESIGNABLE true SCRIPTABLE true)

public:
    explicit SettingsItem(QWidget *parent = 0);

    bool isHead() const;
    void setIsHead(bool head = true);

    bool isTail() const;
    void setIsTail(bool tail = true);

    bool isErr() const;
    void setIsErr(const bool err = true);

    void addBackground();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    bool m_isHead;
    bool m_isTail;
    bool m_isErr;

    DTK_WIDGET_NAMESPACE::DBackgroundGroup *m_bgGroup{nullptr};
};

}
}

#endif // SETTINGSITEM_H
