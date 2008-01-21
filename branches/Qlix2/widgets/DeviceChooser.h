#ifndef __DEVICECHOOSER__
#define __DEVICECHOOSER__
#include <QGridLayout>
#include <QScrollArea>
#include <QGroupBox>
#include <QToolButton>
#include <QSpacerItem>
#include <QGridLayout>
#include <QLabel>
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
  void Reinitialize();

private:

  void setupConnections();
  void initialize();

  QGridLayout* _noDeviceWidget;
  void createNoDeviceWidget();


  QGridLayout* _chooserLayout;
  QGroupBox*  _chooserGroupBox;

  QButtonVector _deviceButtons;
  MtpSubSystem* _subsystem;
};
#endif
