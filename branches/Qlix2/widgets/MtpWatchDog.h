#ifndef __MTPWATCHDOG__
#define __MTPWATCHDOG__
#include <QThread>
#include <QMutex>
#include <QtDebug>
#include <QVector>
#include "mtp/MtpSubSystem.h"
#include "types.h"
#include "widgets/QMtpDevice.h"

class QMtpDevice;
class MtpWatchDog : public QThread
{
  Q_OBJECT
public:
  MtpWatchDog (MtpSubSystem*, QObject* parent = NULL);
  void Lock();
  void Unlock();

signals:
  void DeviceCountChanged(count_t );
  void NewDevice(QMtpDevice* Device);

protected:
  void run();

private:
  QMutex _subSystemLock;
  MtpSubSystem* _subSystem;
  count_t _deviceCount;
};
#endif
