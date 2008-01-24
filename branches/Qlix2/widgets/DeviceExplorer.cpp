#include "DeviceExplorer.h"

DeviceExplorer::DeviceExplorer(QMtpDevice* in_device, QWidget* parent) :
                              _device(in_device)
{
  _layout = new QHBoxLayout(this);
  //setup the device view
  _albumModel = _device->GetAlbumModel();
/*
  _dirModel= _device->GetDirModel();
*/
  _sortedModel = new QSortFilterProxyModel();
  _sortedModel->setSourceModel(_albumModel);

  _deviceView = new QTreeView();
//  _deviceView->setModel(_dirModel);
  _deviceView->setModel(_sortedModel);
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

  _fsModel->setSorting( QDir::Name | QDir::DirsFirst);
  _fsModel->setFilter( QDir::Files | QDir::Dirs | QDir::Hidden);
  connect(_fsView, SIGNAL( doubleClicked ( const QModelIndex& )),
          _fsView, SLOT( setRootIndex (const QModelIndex&) ) );
}
