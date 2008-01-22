#ifndef __QMTPDEVICE__
#define __QMTPDEVICE__
#include <QThread>
#include <QIcon>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QQueue>

#include "mtp/MtpDevice.h"
#include "widgets/MtpWatchDog.h"
#include "widgets/CommandCodes.h"
class MtpWatchDog;
/*
 * A threaded version of MtpDevice representing device attributes using QT 
 * objects and models
 */
class QMtpDevice : public QThread
{
  Q_OBJECT
public:
  QMtpDevice(MtpDevice*, MtpWatchDog*, QObject* parent = NULL);
  QString Name();
  QIcon Icon();
  void IssueCommand (MtpCommand* in_command);

signals:
  void Initialized(QMtpDevice*);

protected:
  void run();

private:

  void QMtpDevice::findAndRetreiveDeviceIcon();
  void QMtpDevice::initializeDeviceStructures();

  void lockusb();
  void unlockusb();
  MtpDevice* _device;

  MtpWatchDog* _watchDog;
  QIcon _icon;
  QString _name;

  QQueue <MtpCommand*> _jobs;
  QMutex _jobLock;
  QWaitCondition _noJobsCondition;

};
#endif
