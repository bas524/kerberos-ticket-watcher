#ifndef KEYCHAINCLASS_H
#define KEYCHAINCLASS_H

#include <QObject>
#if QT_VERSION_MAJOR == 5
#include <qt5keychain/keychain.h>
#elif QT_VERSION_MAJOR == 6
#include <qt6keychain/keychain.h>
#endif

class KeyChainClass : public QObject {
  Q_OBJECT
 public:
  KeyChainClass(QObject *parent = nullptr);

  QString readKey(const QString &key);

  void writeKey(const QString &key, const QString &value);

  void deleteKey(const QString &key);

 private:
  QKeychain::ReadPasswordJob m_readCredentialJob;
  QKeychain::WritePasswordJob m_writeCredentialJob;
  QKeychain::DeletePasswordJob m_deleteCredentialJob;
};

#endif  // KEYCHAINCLASS_H
