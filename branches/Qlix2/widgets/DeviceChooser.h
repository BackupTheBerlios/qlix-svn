#ifndef __DEVICECHOOSER__
#define __DEVICECHOOSER__
#include <QGridLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QToolButton>
#include <QSpacerItem>
#include <libmtp.h>
#include "types.h"
#include <QDebug>
#include "mtp/MtpSubSystem.h"
#include "widgets/DeviceButton.h"


/* This class displays the device chooser widget */
typedef QVector<DeviceButton*> QButtonVector;
class  DeviceChooser : public QScrollArea
{
Q_OBJECT
public:
  DeviceChooser(QWidget*, MtpSubSystem*);

public slots:
  void ExclusivelySelected(DeviceButton*); 

private:

  void setupConnections();
  QGridLayout* _chooserLayout;
  QGroupBox*  _chooserGroupBox;

  QButtonVector _deviceButtons;
  LIBMTP_mtpdevice_t* _devices;
};
#endif
