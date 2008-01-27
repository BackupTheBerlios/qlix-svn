#include "DeviceExplorer.h"
//TODO context/selection sensative context menus..
/**
 * Constructs a new DeviceExplorer
 * @param in_device the device whos database to represent
 * @param parent the parent widget of this widget, should QlixMainWindow
 */
DeviceExplorer::DeviceExplorer(QMtpDevice* in_device, QWidget* parent) :
                              _device(in_device)
{
  _layout = new QGridLayout(this);

  _preferencesWidget = new QlixPreferences();
  _deviceManagerWidget = new QLabel("Device manager!");
  _preferencesWidget->hide();
  _deviceManagerWidget->hide();

  //Get the models
  _albumModel = _device->GetAlbumModel();
  _dirModel= _device->GetDirModel();
  _plModel = _device->GetPlaylistModel();

  _deviceView = new QTreeView();
  _deviceView->setModel(_albumModel);
  _otherWidgetShown = false;
  _queueShown = false;

  _deviceView->setSortingEnabled(true);
  _deviceView->sortByColumn(0, Qt::AscendingOrder);
  _deviceView->header()->hide();
  _deviceView->setAlternatingRowColors(true);

  //setup the filesystem view
  _fsModel = new QDirModel();
  _fsView = new QListView();
  _fsView->setModel(_fsModel);
  _fsView->setAlternatingRowColors(true);
  _fsView->setSelectionBehavior(QAbstractItemView::SelectRows);
  _fsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _queueView = new QListView();
  _queueView->setModel(_fsModel);
  _queueView->resize(QSize(300,300));

  //setup the quick transfer toolbar
  _quickTools = new QToolBar();
  _quickTools->setOrientation(Qt::Vertical);
  _quickTools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _quickTools->setFloatable(false);
  _quickTools->setIconSize(QSize(12,12));
  _quickTools->setMovable(false);
  _quickTools->setContentsMargins(0,0,0,-3);


  _fsDeviceSplit = new QSplitter();
  _queueSplit = new QSplitter();
  _queueSplit->setOrientation(Qt::Vertical);
  _fsDeviceSplit->setOrientation(Qt::Horizontal);
  //add the widgets to the layout
  _fsDeviceSplit->addWidget(_fsView);
  _fsDeviceSplit->addWidget(_deviceView);
  _queueSplit->addWidget(_fsDeviceSplit);
  _queueSplit->addWidget(_queueView);

  _layout->addWidget(_queueSplit,1,0);
  _queueView->hide();
  _layout->addWidget(_preferencesWidget,1,1);
  _layout->addWidget(_deviceManagerWidget,1,2);

  _fsModel->setSorting( QDir::Name | QDir::DirsFirst);
  _fsModel->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden 
                      |QDir::Readable);
  _fsModel->setLazyChildCount(true);
  _fsView->setRootIndex(_fsModel->index(QDir::currentPath()));

  setupToolBars();
  setupConnections();
  setupMenus();
}


/**
 * Displays the AlbumModel and hides the preferences and devicemManager 
 * widgets 
 */
void DeviceExplorer::ShowAlbums()
{
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->hide();
    _fsView->show();
    _deviceView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }//  _sortedModel->setSourceModel(_albumModel);
  if (_deviceView->model() != _albumModel)
    _deviceView->setModel(_albumModel);

  hidePlaylistTools();
  hideFileTools();
  showAlbumTools();
}

void DeviceExplorer::setupToolBars()
{
  _tools = new QToolBar();
  _tools->setOrientation(Qt::Horizontal);
  _tools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  _tools->setFloatable(false);
  _tools->setIconSize(QSize(12,12));
  _tools->setMovable(false);
  _tools->setContentsMargins(0,0,0,-3);
  _fsDeviceSplit->setContentsMargins(0,-1,0,1);


  _layout->addWidget(_tools, 0,0);
  setupCommonTools();
  setupAlbumTools();
  setupPlaylistTools();
  setupFileTools();
}



bool DeviceExplorer::QueueState() { return _queueShown;}

/**
 * Displays the PlaylistModel and hides the preferences and devicemManager 
 * widgets 
 */
void DeviceExplorer::ShowPlaylists()
{
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->hide();
    _fsView->show();
    _deviceView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }//  _sortedModel->setSourceModel(_plModel);

  if (_deviceView->model() != _plModel)
    _deviceView->setModel(_plModel);
  hideAlbumTools();
  hideFileTools();
  showPlaylistTools();
}

/**
 * Displays the DirModel and hides the preferences and devicemManager 
 * widgets 
 */
void DeviceExplorer::ShowFiles()
{
//  _sortedModel->setSourceModel(_dirModel);
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->hide();
    _fsView->show();
    _deviceView->show();
    _otherWidgetShown = false;
    if (_queueShown)
      _queueView->show();
  }
  if (_deviceView->model() != _dirModel)
    _deviceView->setModel(_dirModel);
    hideAlbumTools();
    hidePlaylistTools();
    showFileTools();
}

/**
 * Displays the preferences widget and hides the filesystem and device views
 * Also hides the devicemanager widget if it is being shown
 */
void DeviceExplorer::ShowPreferences()
{
  if (_otherWidgetShown)
  {
    _deviceManagerWidget->hide();
    _preferencesWidget->show();
  }

  else
  {
    _deviceView->hide();
    _fsView->hide();
    _queueView->hide();
    _queueView->hide();
    _preferencesWidget->show();
    _otherWidgetShown = true;
  }
  _tools->hide();
}

void DeviceExplorer::ShowQueue( bool showQueue )
{
  if (showQueue)
  {
    _queueView->show();
    _queueShown = true;
  }
  else
  {
    _queueView->hide();
    _queueShown = false;
  }
}

/**
 * Displays the DeviceManager widget and hides the filesystem and device views
 * Also hides the preferences widget if it is being shown
 */
void DeviceExplorer::ShowDeviceManager()
{
  if (_otherWidgetShown)
  {
    _preferencesWidget->hide();
    _deviceManagerWidget->show();
  }
  else
  {
    _deviceView->hide();
    _fsView->hide();
    _queueView->hide();
    _deviceManagerWidget->show();
    _otherWidgetShown = true;
  }
  _tools->hide();
}

void DeviceExplorer::setupConnections()
{
  connect(_fsView, SIGNAL( doubleClicked ( const QModelIndex& )),
          _fsView, SLOT( setRootIndex (const QModelIndex&) ) );
  connect(_viewQueue, SIGNAL(triggered(bool)), 
          this, SLOT(ShowQueue(bool)));

  connect(_transferTrackToDevice, SIGNAL(triggered(bool)),
          this, SLOT(TransferTrackToDevice()));
}

void DeviceExplorer::showAlbumTools()
{
  _tools->show();
  for (int i =0; i < _albumActionList.size(); i++)
    _albumActionList[i]->setVisible(true);
  _tools->setMinimumSize(13, 12);
  clearActions();
  _fsView->addAction(_transferTrackToDevice);
  _fsView->addAction(_addToQueue);
  _fsView->addAction(_sync);
  _deviceView->addAction(_transferFromDevice);
  _deviceView->addAction(_delete);
}

void DeviceExplorer::showPlaylistTools()
{
  _tools->show();
  for (int i =0; i< _playlistActionList.size(); i++)
    _playlistActionList[i]->setVisible(true);
  _tools->setMinimumSize(12, 12);
  clearActions();
}

void DeviceExplorer::showFileTools()
{
  for (int i =0; i < _fileActionList.size(); i++)
     _fileActionList[i]->setVisible(true);
  _tools->setMinimumSize(12, 13);
  clearActions();
}

void DeviceExplorer::hideAlbumTools()
{
  for (int i =0; i < _albumActionList.size(); i++)
    _albumActionList[i]->setVisible(false);
}

void DeviceExplorer::hidePlaylistTools()
{
  for (int i =0; i< _playlistActionList.size(); i++)
    _playlistActionList[i]->setVisible(false);
}

void DeviceExplorer::hideFileTools()
{
  for (int i =0; i < _fileActionList.size(); i++)
     _fileActionList[i]->setVisible(false);
}


//Must be done before all others
void DeviceExplorer::setupCommonTools()
{

  _transferFromDevice = new QAction( 
    QIcon(":/pixmaps/ActionBar/TransferFromDevice.png"), 
    QString("Transfer From Device"), NULL); 

  _addToQueue= new QAction( 
    QIcon(":/pixmaps/ActionBar/AddToQueue.png"), 
    QString("Add to queue"), NULL); 

  _viewQueue = new QAction( 
    QIcon(":/pixmaps/ActionBar/ShowQueue.png"), 
    QString("Show Queue"), NULL); 

  _delete = new QAction( 
    QIcon(":/pixmaps/ActionBar/DeleteFile.png"), 
    QString("Delete"), NULL); 

  _sync = new QAction( 
    QIcon(":/pixmaps/ActionBar/Sync Queue.png"), 
    QString("Sync"), NULL); 
}

void DeviceExplorer::setupPlaylistTools()
{
  // The Playlist toolset
  _newPlaylist = new QAction( 
    QIcon(":/pixmaps/ActionBar/NewPlaylist.png"), 
    QString("New Playlist"), NULL); 
  _playlistActionList.push_back(_newPlaylist);

  _showDeviceTracks = new QAction( 
    QIcon(":/pixmaps/ActionBar/ShowDeviceTracks.png"), 
    QString("Device Tracks"), NULL); 
  _playlistActionList.push_back(_showDeviceTracks);

  _showFSTracks = new QAction( 
    QIcon(":/pixmaps/ActionBar/ShowFSTracks.png"), 
    QString("Computer Files"), NULL); 
  _playlistActionList.push_back(_showFSTracks);

  _deletePlaylist = new QAction( 
    QIcon(":/pixmaps/ActionBar/DeletePlaylist.png"), 
    QString("Delete Playlist"), NULL); 
  _playlistActionList.push_back(_deletePlaylist);

  hidePlaylistTools();
  for (int i =0; i< _playlistActionList.size(); i++)
    _tools->addAction(_playlistActionList[i]);
}

void DeviceExplorer::setupFileTools()
{
  //Add common tools
  _fileActionList.push_back(_addToQueue);
  _fileActionList.push_back(_viewQueue);
  _fileActionList.push_back(_delete);
  _fileActionList.push_back(_sync);

  //The Filelist toolset 
  _newFolder = new QAction( 
    QIcon(":/pixmaps/ActionBar/NewFoler.png"), 
    QString("New Folder"), NULL); 
  _fileActionList.push_back(_newFolder);

  hideFileTools();
  for (int i =0; i < _fileActionList.size(); i++)
    _tools->addAction( _fileActionList[i]);
}


void DeviceExplorer::setupAlbumTools()
{
    _transferTrackToDevice = new QAction( 
    QIcon(":/pixmaps/ActionBar/TransferToDevice.png"), 
    QString("Transfer Track Now"), NULL); 

  _albumActionList.push_back(_addToQueue);
  _albumActionList.push_back(_viewQueue);
  _albumActionList.push_back(_sync);

  hideAlbumTools();
  for (int i =0; i< _albumActionList.size(); i++)
    _tools->addAction(_albumActionList[i]);
}

void DeviceExplorer::clearActions()
{
  QList<QAction*> list = _fsView->actions();
  while(list.size() > 0)
  {
    _fsView->removeAction(list.first());
    list.pop_front();
  }
  list = _deviceView->actions();
  while(list.size() > 0)
  {
    _deviceView->removeAction(list.first());
    list.pop_front();
  }
}

void DeviceExplorer::setupMenus()
{


  _fsView->setContextMenuPolicy(Qt::ActionsContextMenu);
  _deviceView->setContextMenuPolicy(Qt::ActionsContextMenu);
  _deviceView->setContextMenuPolicy(Qt::ActionsContextMenu);
  setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DeviceExplorer::TransferTrackToDevice()
{
  qDebug() << "called TRansfer Track to device";
    QList<QString> fileList;
    QItemSelectionModel* selectedModel = _fsView->selectionModel();
    QModelIndexList idxList = selectedModel->selectedRows();
    if (idxList.size() == 0)
    {
      qDebug() << "nothing selected!";
      return;
    }

    while(idxList.size() > 0)
    {
      QString fpath = _fsModel->filePath(idxList.front());
      fileList.push_back(fpath);
      idxList.pop_front();
    }
/*
    selctedModel = _deviceView->selectionModel();
    idxList = selectedModel->selectedRows();
*/
    while (fileList.size() > 0)
    {
      _device->TransferTrack(fileList.front());
      fileList.pop_front();
    }
}
