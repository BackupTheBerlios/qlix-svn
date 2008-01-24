#include <QWidget>
#include <QHBoxLayout>
#include <QDirModel>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QHeaderView>
#include <QDir>
#include <QSortFilterProxyModel>
#include "widgets/QMtpDevice.h"
#include "widgets/AlbumModel.h"
#include "widgets/DirModel.h"
#include "mtp/MtpObject.h"
#include "types.h"


class DeviceExplorer : public QWidget
{
Q_OBJECT
public:
  DeviceExplorer(QMtpDevice*, QWidget* parent);
private:

  QHBoxLayout* _layout; 

//Filesystem representation
  QListView* _fsView;
  QDirModel* _fsModel;

//Device representations
  QMtpDevice* _device;
  QHeaderView* _horizontalHeader;
  QSortFilterProxyModel* _sortedModel;
  QTreeView* _deviceView;
  AlbumModel* _albumModel;
  DirModel* _dirModel;
};
