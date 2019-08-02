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

#include "accountswidget.h"

#include "modules/accounts/usermodel.h"
#include "modules/accounts/user.h"
#include "accountsdetailwidget.h"

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>

#include <QDebug>
#include <QIcon>
#include <QSize>

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AccountsWidget::AccountsWidget(QWidget *parent)
    : QWidget(parent)
    , m_createBtn(new QPushButton(tr("Create Account")))
    , m_userlistView(new QListView)
    , m_userItemModel(new QStandardItemModel)
{
    setObjectName("Accounts");

    QVBoxLayout *mainContentLayout = new QVBoxLayout();
    mainContentLayout->addWidget(m_userlistView);
    mainContentLayout->addWidget(m_createBtn);

    m_userlistView->setIconSize(QSize(30, 30));
    m_userlistView->setLayoutDirection(Qt::LeftToRight);
    m_userlistView->setModel(m_userItemModel);

    setLayout(mainContentLayout);

    connect(m_userlistView, &QListView::clicked, this, &AccountsWidget::onItemClicked);
    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

void AccountsWidget::setModel(UserModel *model)
{
    connect(model, &UserModel::userAdded, this, &AccountsWidget::addUser);
    connect(model, &UserModel::userRemoved, this, &AccountsWidget::removeUser);
    //给账户列表添加用户数据
    for (auto user : model->userList()) {
        addUser(user);
    }
}

void AccountsWidget::addUser(User *user)
{
    QStandardItem *item = new QStandardItem;
    connect(user, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        item->setIcon(QIcon(avatar));
    });
    connect(user, &User::nameChanged, this, [ = ](const QString & name) {
        item->setText(name);
    });
    item->setIcon(QIcon(user->currentAvatar()));
    item->setText(user->name());
    item->setTextAlignment(Qt::AlignCenter);
    item->setEditable(false);
    m_userItemModel->appendRow(item);
    m_userList << user;
}

void AccountsWidget::removeUser(User *user)
{
    m_userItemModel->removeRow(m_userList.indexOf(user)); // It will delete when remove
    m_userList.removeOne(user);
}

void AccountsWidget::onItemClicked(const QModelIndex &index)
{
    Q_EMIT requestShowAccountsDetail(m_userList[index.row()]);
}
