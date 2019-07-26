/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#ifndef MODIFYPASSWDPAGE_H
#define MODIFYPASSWDPAGE_H

#include "window/namespace.h"
#include "modules/accounts/user.h"

#include <QWidget>


using namespace dcc;
using namespace dcc::accounts;



namespace DCC_NAMESPACE {
namespace accounts {

//修改密码页面
class ModifyPasswdPage : public QWidget
{
    Q_OBJECT
public:
    explicit ModifyPasswdPage(User *user, QWidget *parent = nullptr);



Q_SIGNALS:
    void requestChangePassword(User *userInter, const QString &oldPassword, const QString &password);



public Q_SLOTS:



private:
    User *m_curUser;
};

}
}
#endif // MODIFYPASSWDPAGE_H
