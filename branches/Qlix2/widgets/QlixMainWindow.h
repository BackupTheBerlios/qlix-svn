#ifndef __QLIXMAINWINDOW__
#define __QLIXMAINWINDOW__

#include <QMainWindow>
#include <QString>
#include "libmtp.h"
#include "mtp/MtpSubSystem.h"
#include "widgets/DeviceChooser.h"


/* This class first displays the device chooser widget. Then it displays either 
 * the batch-mode transfer widget
 */
class QlixMainWindow : public QMainWindow
{
Q_OBJECT
public:
  QlixMainWindow(MtpSubSystem*);
protected:
//  void closeEvent (QCloseEvent* eventh

private:
  MtpSubSystem* _subSystem;
  DeviceChooser* _deviceChooser;
};

#endif
