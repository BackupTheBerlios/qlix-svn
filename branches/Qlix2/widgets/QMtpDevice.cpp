#include "widgets/QMtpDevice.h"

QMtpDevice::QMtpDevice(MtpDevice* in_device, MtpWatchDog* in_watchDog, 
                       QObject* parent):
                       _device(in_device),
                       _watchDog(in_watchDog),
                       _icon(QPixmap(":/pixmaps/miscDev.png"))
{ 
  start(); 
}

QString QMtpDevice::Name()
{ return _name; }

QIcon QMtpDevice::Icon()
{ return _icon; }

void QMtpDevice::IssueCommand(MtpCommand* in_cmd)
{
  QMutexLocker locker(&_jobLock);
  _jobs.push_back(in_cmd);
}

void QMtpDevice::run()
{
  lockusb();
  initializeDeviceStructures();
  findAndRetreiveDeviceIcon();
  unlockusb();
  emit Initialized(this);

  while (true)
  {
    _jobLock.lock();
    while (_jobs.empty() )
      _noJobsCondition.wait(&_jobLock); 

    MtpCommand* currentJob = _jobs.dequeue();
    _jobLock.unlock();

    MtpCommandCode type = currentJob->GetCommand();
    switch (type)
    {
      case Initialize:
         break;
    }
  }
}

void QMtpDevice::lockusb()
{
  assert(_watchDog);
  _watchDog->Lock();
}

void QMtpDevice::unlockusb()
{
  assert(_watchDog);
  _watchDog->Unlock();
}

/*
 * Initializes the base device which retreives the device friendly name etc
 */
void QMtpDevice::initializeDeviceStructures()
{
  if (!_device)
    return;
  _device->Initialize();
}

/*
 * Iterates over all the devices files and tries to find devIcon.fil
 */
void QMtpDevice::findAndRetreiveDeviceIcon()
{
  count_t fileCount = _device->FileCount();
  count_t thread_id = (int)this;
  QString iconPath = QString("/tmp/%1Icon").arg(thread_id); 
  MTP::File* curFile = NULL;
  for (count_t i = 0; i < fileCount; i++)
  {
    curFile = _device->File(i);
    QString name = QString::fromUtf8(curFile->Name());
    name = name.toLower();
    if (name == "devicon.fil")
      break;
  }
  if (curFile)
    _device->Retreive(curFile->ID(), iconPath.toLatin1());
  _icon = QIcon(QPixmap(iconPath));
}
