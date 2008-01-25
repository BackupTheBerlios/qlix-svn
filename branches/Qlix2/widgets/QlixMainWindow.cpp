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

  _deviceExplorer = new DeviceExplorer(in_device, this);
  setupToolBar();
  setupActions();
  setupConnections();
  setCentralWidget(_deviceExplorer);
}

void QlixMainWindow::setupToolBar()
{
  _toolBar = new QToolBar();
  _toolBar->setOrientation(Qt::Horizontal);
  _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  _toolBar->setFloatable(false);
  addToolBar(Qt::LeftToolBarArea,_toolBar);
}

void QlixMainWindow::setupActions()
{
  _albumlistAction= new QAction( QIcon(":/pixmaps/albumlist.png"), QString("View Albums"), NULL); 
  _playlistAction = new QAction( QIcon(":/pixmaps/playlist.png"), QString("View Playlists"), NULL); 
  _filelistAction = new QAction( QIcon(":/pixmaps/filelist.png"), QString("View Files"), NULL); 
  _preferencesAction = new QAction( QIcon(":/pixmaps/preferences.png"), QString("Preferences"), NULL); 
  _manageDeviceAction = new QAction( QIcon(":/pixmaps/managedevice.png"), QString("Manage Device"), NULL); 
  _toolBar->addAction(_albumlistAction);
  _toolBar->addAction(_playlistAction);
  _toolBar->addAction(_filelistAction);
  _toolBar->addAction(_manageDeviceAction);
  _toolBar->addAction(_preferencesAction);
  _toolBar->setIconSize(QSize(40,40));
}

void QlixMainWindow::setupConnections()
{
  connect(_albumlistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowAlbums()));
  connect(_playlistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowPlaylists()));
  connect(_filelistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowFiles()));
  connect(_manageDeviceAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowDeviceManager()));
  connect(_preferencesAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowPreferences()));
}
