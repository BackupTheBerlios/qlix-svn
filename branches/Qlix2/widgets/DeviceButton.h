#ifndef __DEVICEBUTTON__
#define __DEVICEBUTTON__
#include <QToolButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QString>
#include <QSpacerItem>
#include "widgets/QMtpDevice.h"
/** 
 * @class DeviceButton Class that displays the device icon and auto connect box
*/
class DeviceButton : public QVBoxLayout
{
  Q_OBJECT
public:
  DeviceButton(QMtpDevice*, QWidget* temp = NULL);
  void RemoveCheck();

signals:
  void Checked(DeviceButton*, QMtpDevice* _device);
  void Selected(QMtpDevice*);

private slots:
  void stateChanged(int);
  void buttonClicked();
private:
  void setupConnections();
  QMtpDevice* _device;
  QCheckBox* _checkBox;
  QToolButton* _button;
};
#endif
