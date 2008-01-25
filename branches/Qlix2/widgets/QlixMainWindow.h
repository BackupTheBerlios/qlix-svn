#ifndef __QLIXMAINWINDOW__
#define __QLIXMAINWINDOW__

#include <QMainWindow>
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

private:
  
  void setupActions();
  void setupToolBar();
  void setupWatchDogConnections();
  void setupConnections();

  MtpWatchDog* _watchDog;
  DeviceChooser* _deviceChooser;
  QMtpDevice* _currentDevice;
  DeviceExplorer* _deviceExplorer;

  QToolBar* _toolBar;
  //Actions
  QAction* _albumlistAction;
  QAction* _filelistAction;
  QAction* _playlistAction;

  QAction* _manageDeviceAction;
  QAction* _preferencesAction;
};

#endif
