#include "widgets/MtpWatchDog.h"
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
  for (count_t i = 0; i< _subSystem->DeviceCount(); i++)
  {
    QMtpDevice* _threadedDev = new QMtpDevice(_subSystem->Device(i), this);
    connect(_threadedDev, SIGNAL(Initialized (QMtpDevice*)),
            this, SIGNAL(NewDevice (QMtpDevice*)), Qt::QueuedConnection);
  }
  Unlock();

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
