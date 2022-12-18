#include <QDebug>
#include <QEventLoop>
#include "keychainclass.h"

KeyChainClass::KeyChainClass(QObject *parent)
    : QObject(parent),
      m_readCredentialJob(QLatin1String("keychain.krb5tw.project.app")),
      m_writeCredentialJob(QLatin1String("keychain.krb5tw.project.app")),
      m_deleteCredentialJob(QLatin1String("keychain.krb5tw.project.app")) {
  m_readCredentialJob.setAutoDelete(false);
  m_writeCredentialJob.setAutoDelete(false);
  m_deleteCredentialJob.setAutoDelete(false);
}

QString KeyChainClass::readKey(const QString &key) {
  QString result;
  m_readCredentialJob.setKey(key);

  QEventLoop loop;
  m_readCredentialJob.connect(&m_readCredentialJob, &QKeychain::ReadPasswordJob::finished, &loop, &QEventLoop::quit);

  m_readCredentialJob.start();
  loop.exec();

  if (m_readCredentialJob.error()) {
    qWarning() << (tr("Read key failed: %1").arg(m_readCredentialJob.errorString()));
  } else {
    result = m_readCredentialJob.textData();
  }
  return result;
}

void KeyChainClass::writeKey(const QString &key, const QString &value) {
  m_writeCredentialJob.setKey(key);
  m_writeCredentialJob.setTextData(value);

  QEventLoop loop;
  m_writeCredentialJob.connect(&m_writeCredentialJob, &QKeychain::WritePasswordJob::finished, &loop, &QEventLoop::quit);
  m_writeCredentialJob.start();
  loop.exec();

  if (m_writeCredentialJob.error()) {
    qWarning() << (tr("Write key failed: %1").arg(qPrintable(m_writeCredentialJob.errorString())));
  }
}

void KeyChainClass::deleteKey(const QString &key) {
  m_deleteCredentialJob.setKey(key);

  QEventLoop loop;
  m_deleteCredentialJob.connect(&m_deleteCredentialJob, &QKeychain::WritePasswordJob::finished, &loop, &QEventLoop::quit);
  m_deleteCredentialJob.start();
  loop.exec();

  if (m_deleteCredentialJob.error()) {
    qWarning() << (tr("Delete key failed: %1").arg(qPrintable(m_deleteCredentialJob.errorString())));
  }
}
