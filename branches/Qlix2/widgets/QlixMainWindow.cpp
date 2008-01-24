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
  connect (_deviceChooser, SIGNAL(DeviceSelected(QMtpDevice*)),
           this, SLOT(DeviceSelected(QMtpDevice*)));
}

void QlixMainWindow::setupWatchDogConnections()
{
  connect(_watchDog, SIGNAL(NewDevice(QMtpDevice*)),
          _deviceChooser, SLOT(AddDevice(QMtpDevice*)), Qt::QueuedConnection);
}

void QlixMainWindow::DeviceSelected(QMtpDevice* in_device)
{
#ifdef QLIX_DEBUG
  qDebug() << "Device selected: " << in_device;
#endif
  _deviceChooser->hide();
  _currentDevice = in_device;
  setupToolBar();
  _deviceExplorer = new DeviceExplorer(in_device, this);
  setCentralWidget(_deviceExplorer);
}

void QlixMainWindow::setupToolBar()
{
  _toolBar = new QToolBar();
  _toolBar->setOrientation(Qt::Horizontal);
  _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  _toolBar->setFloatable(false);
  setupActions();
  addToolBar(Qt::LeftToolBarArea,_toolBar);
}

void QlixMainWindow::setupActions()
{
  _albumListAction= new QAction( QIcon(":/pixmaps/albumList.png"), QString("View Albums"), NULL); 
  _toolBar->addAction(_albumListAction);
}
