#ifndef KEYCHAINCLASS_H
#define KEYCHAINCLASS_H

#include <QObject>

#include <qt5keychain/keychain.h>

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
