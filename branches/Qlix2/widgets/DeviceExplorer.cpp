#include "DeviceExplorer.h"
/**
 * Constructs a new DeviceExplorer
 * @param in_device the device whos database to represent
 * @param parent the parent widget of this widget, should QlixMainWindow
 */
DeviceExplorer::DeviceExplorer(QMtpDevice* in_device, QWidget* parent) :
                              _device(in_device)
{
  _layout = new QHBoxLayout(this);

  _preferencesWidget = new QLabel("preferences!");
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

  _deviceView->setSortingEnabled(true);
  _deviceView->sortByColumn(0, Qt::AscendingOrder);
  _deviceView->header()->resizeSection(0, 200);

  //setup the filesystem view
  _fsModel = new QDirModel();
  _fsView = new QListView();
  _fsView->setModel(_fsModel);

  //add the widgets to the layout
  _layout->addWidget(_fsView);
  _layout->addWidget(_deviceView);
  _layout->addWidget(_preferencesWidget);
  _layout->addWidget(_deviceManagerWidget);

  _fsModel->setSorting( QDir::Name | QDir::DirsFirst);
  _fsModel->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden 
                      |QDir::Readable);
  _fsModel->setLazyChildCount(true);
  _fsView->setRootIndex(_fsModel->index(QDir::currentPath()));
  connect(_fsView, SIGNAL( doubleClicked ( const QModelIndex& )),
          _fsView, SLOT( setRootIndex (const QModelIndex&) ) );
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
  }//  _sortedModel->setSourceModel(_albumModel);
  if (_deviceView->model() != _albumModel)
    _deviceView->setModel(_albumModel);
}

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
  }//  _sortedModel->setSourceModel(_plModel);

  if (_deviceView->model() != _plModel)
    _deviceView->setModel(_plModel);
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
  }
  if (_deviceView->model() != _dirModel)
    _deviceView->setModel(_dirModel);

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
    _preferencesWidget->show();
    _otherWidgetShown = true;
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
    _deviceManagerWidget->show();
    _otherWidgetShown = true;
  }
}
