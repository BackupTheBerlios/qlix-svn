#include <QWidget>
#include <QHBoxLayout>
#include <QDirModel>
#include <QTreeView>
#include <QListView>
#include <QTableView>
#include <QHeaderView>
#include <QDir>
#include <QLabel>
#include <QSortFilterProxyModel>
#include "widgets/QMtpDevice.h"
#include "widgets/AlbumModel.h"
#include "widgets/DirModel.h"
#include "widgets/PlaylistModel.h"
#include "mtp/MtpObject.h"
#include "types.h"

/**
 * @class The DeviceExplorer widget generally will show two panes
 * one will expose the local filesystem, the other will expose the device
 * database. In specific instances it will display the preferences widget
 * and the device management widget. Over all this widget ties all 
 * functionality of Qlix into one main widget. It is the central widget 
 * that is set in QlixMainWindow
 */
class DeviceExplorer : public QWidget
{
Q_OBJECT
public:
  DeviceExplorer(QMtpDevice*, QWidget* parent);

public slots:
  void ShowFiles();
  void ShowAlbums();
  void ShowPlaylists();
  void ShowPreferences();
  void ShowDeviceManager();

private:

  QHBoxLayout* _layout; 

//Filesystem representation
  QListView* _fsView;
  QDirModel* _fsModel;

//Device representations
  QMtpDevice* _device;
  QHeaderView* _horizontalHeader;

  QTreeView* _deviceView;
  QSortFilterProxyModel* _albumModel;
  QSortFilterProxyModel* _plModel;
  QSortFilterProxyModel* _dirModel;
//Device manager and preferences (for now there are just labels
  QLabel* _preferencesWidget;
  QLabel* _deviceManagerWidget;
//Used to figure out if we are displaying the preferences or devicemanager widget
  bool _otherWidgetShown;
};
