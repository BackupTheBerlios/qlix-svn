#ifndef __DEVICECHOOSER__
#define __DEVICECHOOSER__
#include <QGridLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QToolButton>
#include <QSpacerItem>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QMetaType>
#include <QVector>

#include <libmtp.h>
#include "types.h"
#include "mtp/MtpSubSystem.h"
#include "widgets/DeviceButton.h"
#include "widgets/MtpWatchDog.h"
#include "widgets/QMtpDevice.h"


/* This class displays the device chooser widget */
typedef QVector<DeviceButton*> QButtonVector;
class  DeviceChooser : public QScrollArea
{
Q_OBJECT
public:
  DeviceChooser(QWidget*);

public slots:
  void ExclusivelySelected(DeviceButton*); 
  void Reinitialize();
  void AddDevice(QMtpDevice*);

private slots:
  void deviceCountChanged();

private:

  void addButton(DeviceButton*);
  void setupConnections();
  // Connects the WatchDog to this widget to inform it of updates
  void setupWatchDogConnections();
  void initialize();
  void createNoDeviceWidget();
  void addButtonToLayout(DeviceButton*);
  void showNoDevices();


  QVector <QMtpDevice*> _devices;
  /* Used when there are no devices */
  QVector <QWidget*> _noDeviceLayoutList;
  QSpacerItem* _bottomNoDeviceSpacer;
  QSpacerItem* _topNoDeviceSpacer;

  /* Used when devices are detected */
  QButtonVector _deviceButtons;
  QGridLayout* _chooserLayout;
  QGroupBox*  _chooserGroupBox;

  //Used to figure out if the slot deviceCountChanged()
  //has been called atleast once 
  bool _initialized;
  count_t _deviceCount;

};
#endif
