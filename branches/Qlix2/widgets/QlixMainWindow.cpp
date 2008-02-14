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
  connect(_watchDog, SIGNAL(DefaultDevice(QMtpDevice*)),
          this, SLOT(DeviceSelected(QMtpDevice*)), Qt::QueuedConnection);
  connect(_watchDog, SIGNAL(NewDevice(QMtpDevice*)),
          _deviceChooser, SLOT(AddDevice(QMtpDevice*)), Qt::QueuedConnection);
  connect(_watchDog, SIGNAL(NoDevices()),
          _deviceChooser, SLOT(NoDevices()), Qt::QueuedConnection);
}

void QlixMainWindow::DeviceSelected(QMtpDevice* in_device)
{
#ifdef QLIX_DEBUG
  qDebug() << "Device selected: " << in_device;
#endif
  setupStatusBar();
  _deviceChooser->hide();
  _currentDevice = in_device;

  _deviceExplorer = new DeviceExplorer(in_device, this);
  _deviceExplorer->SetProgressBar(_progressBar);
  setupToolBar();
  setupActions();
  setupConnections();
  setCentralWidget(_deviceExplorer);
  _deviceExplorer->setContentsMargins(0,-10,0,0);
  _albumlistAction->trigger();
}

void QlixMainWindow::setupStatusBar()
{
  statusBar()->setMaximumHeight(24);
  statusBar()->setContentsMargins(0,-2,0,0);
  _progressBar = new QProgressBar();
  _progressBar->setTextVisible(false);
  _progressBar->setRange(0, 100);
  _progressBar->setMaximumSize(240, 20);
  _progressBar->setMinimumSize(240, 20);
  _progressBar->setAlignment(Qt::AlignRight);
  statusBar()->insertPermanentWidget(0, _progressBar, 0);
  _progressBar->hide();
}

void QlixMainWindow::setupToolBar()
{
  _navBar = new QToolBar();
  _navBar->setOrientation(Qt::Vertical);
  _navBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  _navBar->setFloatable(false);
/*
  _playlistTools = new QToolBar(_navBar);
  _playlistTools->setOrientation(Qt::Vertical);
  _playlistTools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _playlistTools->setFloatable(false);
  _playlistTools->setIconSize(QSize(12,12));
  _playlistTools->setMovable(false);

  _fileTools = new QToolBar(_navBar);
  _fileTools->setOrientation(Qt::Vertical);
  _fileTools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _fileTools->setFloatable(false);
  _fileTools->setIconSize(QSize(12,12));
  _fileTools->setMovable(false);
  */

  addToolBar(Qt::TopToolBarArea,_navBar);
  /*
  addToolBar(Qt::BottomToolBarArea, _albumTools);
  addToolBar(Qt::BottomToolBarArea, _playlistTools);
  addToolBar(Qt::BottomToolBarArea, _fileTools);
  */
}

void QlixMainWindow::setupActions()
{
  _albumlistAction= new QAction( QIcon(":/pixmaps/albumlist.png"), QString("View Albums"), NULL); 
  _playlistAction = new QAction( QIcon(":/pixmaps/playlist.png"), QString("View Playlists"), NULL); 
  _filelistAction = new QAction( QIcon(":/pixmaps/filelist.png"), QString("View Files"), NULL); 
  _preferencesAction = new QAction( QIcon(":/pixmaps/preferences.png"), QString("Preferences"), NULL); 
  _manageDeviceAction = new QAction( QIcon(":/pixmaps/managedevice.png"), QString("Manage Device"), NULL); 
  _navBar->addAction(_albumlistAction);
  _navBar->addAction(_playlistAction);
  _navBar->addAction(_filelistAction);
  _navBar->addAction(_manageDeviceAction);
  _navBar->addAction(_preferencesAction);

  _navBar->setIconSize(QSize(32,32));

//  setupAlbumActions();
//  setupPlaylistActions();
//  setupFileActions();
}
/*


void QlixMainWindow::showAlbumActions()
{
  _playlistTools->hide();
  _fileTools->hide();
  _albumTools->show();
  _currentView = Albums;
  return;
}

void QlixMainWindow::hideAlbumActions()
{}

void QlixMainWindow::showPlaylistActions()
{
  _albumTools->hide();
  _fileTools->hide();
  _playlistTools->show();
  _currentView = Playlists;
  return;
}

void QlixMainWindow::hidePlaylistActions()
{}


void QlixMainWindow::showFileActions()
{
  _playlistTools->hide();
  _albumTools->hide();
  _fileTools->show();
  _currentView = Files;
  return;
}

void QlixMainWindow::hideFileActions()
{
  for (int i =0; i < _fileActionList.size(); i++)
    _fileActionList[i]->setVisible(false);
}
*/
void QlixMainWindow::setupConnections()
{
  //The following connections change DeviceExplorer's views
  //And show the toolbar
  //And show the right actions
  connect(_albumlistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowAlbums()));

  connect(_playlistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowPlaylists()));
  
  connect(_filelistAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowFiles()));
 
  //The following show the preferences menu and the device manager
  //And hides the toolbar

  connect(_manageDeviceAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowDeviceManager()));
  connect(_preferencesAction, SIGNAL(triggered(bool)),
          _deviceExplorer, SLOT(ShowPreferences()));

}
