#ifndef __QLIXMAINWINDOW__
#define __QLIXMAINWINDOW__

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QIcon>
#include "libmtp.h"
#include "mtp/MtpSubSystem.h"
#include "widgets/DeviceChooser.h"
#include "widgets/DeviceExplorer.h"


/* This class first displays the device chooser widget. Then it displays either 
 * the batch-mode transfer widget
 */
class QlixMainWindow : public QMainWindow
{
Q_OBJECT
public:
  QlixMainWindow(MtpSubSystem*);

public slots:
  void DeviceSelected(QMtpDevice*);
  
protected:
//  void closeEvent (QCloseEvent* eventh

private slots:
/*
  void showAlbumActions();
  void hideAlbumActions();

  void showPlaylistActions();
  void hidePlaylistActions();

  void showFileActions();
  void hideFileActions();
  */

private:
  enum ToolbarView
  {
    Invalid,
    Albums,
    Playlists,
    Files,
    DeviceManager,
    Preferences
  };
  
  void setupActions();
  void setupToolBar();
  void setupWatchDogConnections();
  void setupConnections();


  void setupAlbumActions();
  void setupPlaylistActions();
  void setupFileActions();



  ToolbarView _currentView;

  MtpWatchDog* _watchDog;
  DeviceChooser* _deviceChooser;
  QMtpDevice* _currentDevice;
  DeviceExplorer* _deviceExplorer;

    QToolBar* _navBar;

  //View Actions
  QAction* _albumlistAction;
  QAction* _filelistAction;
  QAction* _playlistAction;
  QAction* _manageDeviceAction;
  QAction* _preferencesAction;

  /*
  //File Actions
  QAction* _transferFile;
  QAction* _deleteFile;
  QAction* _newFolder;
  QAction* _deleteFolder;
  QVector <QAction*> _fileActionList;

*/
};

#endif
