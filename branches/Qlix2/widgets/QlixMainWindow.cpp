#include "QlixMainWindow.h"

QlixMainWindow::QlixMainWindow(MtpSubSystem* in_subsystem)
{

  setMinimumSize(800,500);
  _subSystem = in_subsystem;
  _deviceChooser = new DeviceChooser(this, _subSystem);
  _deviceChooser->resize(frameSize());
  setCentralWidget(_deviceChooser);
}
