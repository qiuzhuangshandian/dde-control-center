/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "systemlanguagesettingwidget.h"
#include "modules/keyboard/keyboardmodel.h"
#include "modules/keyboard/langwidget.h"
#include "modules/keyboard/indexdelegate.h"
#include "modules/keyboard/indexview.h"
#include "modules/keyboard/indexmodel.h"
#include "widgets/searchinput.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>
#include <QEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::keyboard;
using namespace dcc;
using namespace dcc::keyboard;

SystemLanguageSettingWidget::SystemLanguageSettingWidget(KeyboardModel *model, QWidget *parent)
    : ContentWidget(parent)
    , m_keyboardModel(model)
{
    m_contentWidget = new TranslucentFrame();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    m_searchModel = new IndexModel();

    m_model = new IndexModel();
    m_view = new IndexView();
    m_delegate = new IndexDelegate();

    m_search = new SearchInput();
    m_contentTopLayout->addSpacing(10);
    m_contentTopLayout->addWidget(m_search);
    m_contentTopLayout->addSpacing(10);

    layout->addWidget(m_view);
    m_contentWidget->setLayout(layout);

    // 矩形圆角
    m_clipEffectWidget = new DGraphicsClipEffect(m_contentWidget);
    m_contentWidget->installEventFilter(this);
    m_contentWidget->setGraphicsEffect(m_clipEffectWidget);

    setContent(m_contentWidget);
    m_contentWidget->setAttribute(Qt::WA_TranslucentBackground);

    connect(m_search, SIGNAL(textChanged(QString)), this, SLOT(onSearch(QString)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SIGNAL(click(QModelIndex)));

    connect(m_keyboardModel, &KeyboardModel::langChanged, this, &SystemLanguageSettingWidget::setModelData);
    connect(m_keyboardModel, &KeyboardModel::curLangChanged, this, &SystemLanguageSettingWidget::setCurLang);

    setModelData(m_keyboardModel->langLists());
    setCurLang(m_keyboardModel->curLang());
}

void SystemLanguageSettingWidget::setModelData(const QList<MetaData> &datas)
{
    m_datas = datas;
    m_model->setMetaData(datas);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
}

void SystemLanguageSettingWidget::setCurLang(const QString &lang)
{
    for (int i = 0; i < m_view->model()->rowCount(); ++i) {
        MetaData md = m_view->model()->index(i, 0).data().value<MetaData>();
        if (md.text() == lang) {
            m_view->setCurrentIndex(m_view->model()->index(i, 0));
            break;
        }
    }
}

void SystemLanguageSettingWidget::onSearch(const QString &text)
{
    if (text.length() == 0) {
        m_view->setModel(m_model);
    } else {
        QList<MetaData> datas = m_model->metaData();
        QList<MetaData>::iterator it = datas.begin();
        QList<MetaData> sdatas;
        for (; it != datas.end(); ++it) {
            if ((*it).text().contains(text, Qt::CaseInsensitive)) {
                sdatas.append(*it);
            }
        }
        m_searchModel->setMetaData(sdatas);
        m_view->setModel(m_searchModel);
    }

    setCurLang(m_keyboardModel->curLang());
}

bool SystemLanguageSettingWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if (event->type() != QEvent::Move && event->type() != QEvent::Resize)
        return false;

    QRect rect = m_contentWidget->rect();

    rect.moveTopLeft(-m_contentWidget->pos());
    rect.setHeight(m_contentWidget->window()->height() - m_contentWidget->mapTo(window(), rect.topLeft()).y());

    QPainterPath path;
    path.addRoundedRect(rect, 5, 5);
    m_clipEffectWidget->setClipPath(path);

    return false;
}
