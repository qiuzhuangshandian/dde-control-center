#include "secrethotspotsection.h"
#include "contentwidget.h"

#include <networkmanagerqt/utils.h>

using namespace dcc::widgets;

static const QList<NetworkManager::WirelessSecuritySetting::KeyMgmt> KeyMgmtList {
    NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone,
    NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep,
    NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk,
};
static const QMap<QString, NetworkManager::WirelessSecuritySetting::KeyMgmt> KeyMgmtStrMap {
    {"None", NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone},
    {"WEP 40/128-bit", NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep},
    {"WPA/WPA2 Personal", NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk},
};

SecretHotspotSection::SecretHotspotSection(NetworkManager::WirelessSecuritySetting::Ptr wsSeting, QFrame *parent)
    : AbstractSection(parent),
      m_keyMgmtChooser(new ComboBoxWidget(this)),
      m_passwdEdit(new PasswdEditWidget(this)),
      m_currentKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone),
      m_wsSetting(wsSeting)
{
    // init KeyMgmt
    const NetworkManager::WirelessSecuritySetting::KeyMgmt &keyMgmt = m_wsSetting->keyMgmt();
    m_currentKeyMgmt = (keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Unknown) ?
        NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone : keyMgmt;

    initUI();
    initConnection();

    onKeyMgmtChanged(m_currentKeyMgmt);
}

SecretHotspotSection::~SecretHotspotSection()
{
}

bool SecretHotspotSection::allInputValid()
{
    bool valid = true;

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        valid = NetworkManager::wepKeyIsValid(m_passwdEdit->text(),
                NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
        m_passwdEdit->setIsErr(!valid);
    }

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        valid = NetworkManager::wpaPskIsValid(m_passwdEdit->text());
        m_passwdEdit->setIsErr(!valid);
    }

    return valid;
}

void SecretHotspotSection::saveSettings()
{
    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone) {
        m_wsSetting->setInitialized(false);
        return;
    }

    m_wsSetting->setKeyMgmt(m_currentKeyMgmt);
    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        m_wsSetting->setWepKeyType(NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
        m_wsSetting->setWepKey0(m_passwdEdit->text());
    } else if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        m_wsSetting->setPsk(m_passwdEdit->text());
    }

    m_wsSetting->setInitialized(true);
}

void SecretHotspotSection::initUI()
{
    m_keyMgmtChooser->setTitle(tr("Security"));
    for (auto keyMgmt : KeyMgmtList) {
        m_keyMgmtChooser->appendOption(KeyMgmtStrMap.key(keyMgmt), keyMgmt);
    }
    m_keyMgmtChooser->setCurrent(m_currentKeyMgmt);

    m_passwdEdit->setPlaceholderText(tr("Required"));

    appendItem(m_keyMgmtChooser);
    appendItem(m_passwdEdit);
}

void SecretHotspotSection::initConnection()
{
    connect(m_keyMgmtChooser, &ComboBoxWidget::requestPage, this, &SecretHotspotSection::requestPage);
    connect(m_keyMgmtChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onKeyMgmtChanged(data.value<NetworkManager::WirelessSecuritySetting::KeyMgmt>());
    });

    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretHotspotSection::saveUserInputPassword);
    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretHotspotSection::allInputValid);
}

void SecretHotspotSection::onKeyMgmtChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt)
{
    if (m_currentKeyMgmt != keyMgmt) {
        m_currentKeyMgmt = keyMgmt;
    }

    switch (m_currentKeyMgmt) {
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone: {
            m_passwdEdit->setVisible(false);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep: {
            m_passwdEdit->setText(m_wsSetting->wepKey0());
            m_passwdEdit->setTitle(tr("Key"));
            m_passwdEdit->setVisible(true);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk: {
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(true);
            break;
        }
        default:
            break;
    }

    if (m_userInputPasswordMap.contains(m_currentKeyMgmt)) {
        m_passwdEdit->setText(m_userInputPasswordMap.value(m_currentKeyMgmt));
    }
}

void SecretHotspotSection::saveUserInputPassword()
{
    m_userInputPasswordMap.insert(m_currentKeyMgmt, m_passwdEdit->text());
}
