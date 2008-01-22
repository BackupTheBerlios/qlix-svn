#include "QlixMainWindow.h"

QlixMainWindow::QlixMainWindow(MtpSubSystem* in_subsystem)
{

  setMinimumSize(800,500);
  _watchDog = new MtpWatchDog(in_subsystem);
  _deviceChooser = new DeviceChooser(this);
  setupWatchDogConnections();
  _watchDog->start();
  _deviceChooser->resize(frameSize());
  setCentralWidget(_deviceChooser);
}

void QlixMainWindow::setupWatchDogConnections()
{
  connect(_watchDog, SIGNAL(NewDevice(QMtpDevice*)),
          _deviceChooser, SLOT(AddDevice(QMtpDevice*)), Qt::QueuedConnection);
}
