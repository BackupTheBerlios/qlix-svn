#include <QWidget>
#include <QToolBar>
#include <QSpacerItem>
#include <QProgressBar>
#include <QAction>
#include <QToolButton>
#include <QGridLayout>
#include <QDirModel>
#include <QTreeView>
#include <QListView>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QSplitter>
#include <QDir>
#include <QLabel>
#include "widgets/QlixPreferences.h"
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
  bool QueueState();
  void SetProgressBar(QProgressBar*);

public slots:
  void ShowFiles();
  void ShowAlbums();
  void ShowPlaylists();
  void ShowPreferences();
  void ShowDeviceManager();
  void ShowQueue(bool);
  void UpdateProgressBar(const QString&, count_t percent);

signals:

private slots:
  void TransferTrackToDevice();
  void TransferFromDevice();
private:
  enum ViewPort
  {
    AlbumsView,
    PlaylistView,
    FileView
  };
  void setupToolBars();
  void setupMenus();
  void setupDeviceView();
  void setupFileSystemView();
  void setupProgressBar();

  void setupAlbumTools();
  void setupPlaylistTools();
  void setupFileTools();
  void setupCommonTools();

  void setupConnections();
  void showAlbumTools();
  void showPlaylistTools();
  void showFileTools();
  void hideAlbumTools();
  void hidePlaylistTools();
  void hideFileTools();

  void updateDeviceSpace();
  void clearActions();
  QGridLayout* _layout; 
  ViewPort _view;

//Filesystem representation
  QListView* _fsView;
  QDirModel* _fsModel;

//Device representations
  QMtpDevice* _device;
  QHeaderView* _horizontalHeader;

//Queue representation
  QListView* _queueView;

  QTreeView* _deviceView;
  QAbstractItemModel* _albumModel;
  QSortFilterProxyModel* _plModel;
  QSortFilterProxyModel* _dirModel;

  QSplitter* _fsDeviceSplit;
  QSplitter* _queueSplit;
//Device manager and preferences (for now there are just labels
  QlixPreferences* _preferencesWidget;
  QLabel* _deviceManagerWidget;
//Used to figure out if we are displaying the preferences or devicemanager
//widget
  bool _otherWidgetShown;
  bool _queueShown;

  //Track toolbar and actions
  QToolBar* _tools;
  QSpacerItem* _progressBarSpacer;
  //Progress bar for space usage and transfers
  QProgressBar* _progressBar;

  //Album Actions
  QAction* _transferTrackToDevice;
  QVector <QAction*> _albumActionList;

  //Playlist Actions
  QAction* _newPlaylist;
  QAction* _addToPlaylist;
  QAction* _showDeviceTracks;
  QAction* _showFSTracks;
  QAction* _deletePlaylist;
  QAction* _deleteFromPlaylist;
  QVector <QAction*> _playlistActionList;

  //File Actions
  QAction* _newFolder;
  QVector <QAction*> _fileActionList;

  //common to file and playlist
  QAction* _delete;
  QAction* _transferFromDevice;
  QAction* _addToQueue;
  QAction* _viewQueue;
  QAction* _sync;
};

