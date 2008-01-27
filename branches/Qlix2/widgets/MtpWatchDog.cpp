#include "widgets/MtpWatchDog.h"

/** 
 * Creates a new WatchDog over the given subsystem
 * @param in_subSystem the subsystem to watch over
 */
MtpWatchDog::MtpWatchDog(MtpSubSystem* in_subSystem, QObject* parent) :
                         _subSystem(in_subSystem),
                         _deviceCount(0)
{ }

/** 
 * Begin running the MtpWatchDog by polling LIBMTP for new devices
 */
void MtpWatchDog::run()
{
  Lock();
  _subSystem->Initialize();
  QSettings settings;
  QString defaultDev = settings.value("DefaultDevice").toString();

  if (_subSystem->DeviceCount() == 0)
  {
    emit NoDevices();
    return;
  }

  for (count_t i = 0; i< _subSystem->DeviceCount() && !defaultDev.isEmpty(); i++)
  {
    if (QString::fromUtf8(_subSystem->Device(i)->SerialNumber()) == defaultDev)
    {
      QMtpDevice* _threadedDev = new QMtpDevice(_subSystem->Device(i), this);
      connect(_threadedDev, SIGNAL(Initialized (QMtpDevice*)),
              this, SIGNAL(DefaultDevice(QMtpDevice*)), Qt::QueuedConnection);
      qDebug() << "Found the defualt device: " << defaultDev;
      Unlock();
      return;
    }
  }

  for (count_t i = 0; i< _subSystem->DeviceCount(); i++)
  {
    QMtpDevice* _threadedDev = new QMtpDevice(_subSystem->Device(i), this);
    connect(_threadedDev, SIGNAL(Initialized (QMtpDevice*)),
            this, SIGNAL(NewDevice (QMtpDevice*)), Qt::QueuedConnection);
  }
  Unlock();
// Currently libmtp does not support polling the device interface so for now 
// this is allthis function does, that is, create QMtpDevices for discovered devices
//  while (true)
//  {  }
}

/** 
 * One must lock up the WatchDog before working with the MTP subsystem
 */
void MtpWatchDog::Lock()
{
  _subSystemLock.lock();
}

/** 
 * One must unlock the WatchDog once done working with the MTP subsystem
 */
void MtpWatchDog::Unlock()
{
  _subSystemLock.unlock();
}
