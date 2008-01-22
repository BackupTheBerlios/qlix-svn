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
  //private forward declration
  class NoDevice;

  void addButton(DeviceButton*);
  void setupConnections(count_t idx);
  // Connects the WatchDog to this widget to inform it of updates
  void setupWatchDogConnections();
  void initialize();
  void createNoDeviceWidget();
  void addButtonToLayout(DeviceButton*);
  void showNoDeviceWidget();
  void hideNoDeviceWidget();


  QVector <QMtpDevice*> _devices;
  /* Used when there are no devices */
  NoDevice* _noDeviceLayout;

  /* Used when devices are detected */
  QButtonVector _deviceButtons;
  QGridLayout* _chooserLayout;
  QGroupBox*  _chooserGroupBox;

  /**
  * @class A private class to display the NoDevice Widget
  */
  class NoDevice : public QWidget 
  {
    public:
    NoDevice(QWidget* parent = NULL)
    {
      _layout = new QGridLayout();
      _top = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                              QSizePolicy::Expanding);
      _bottom = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                              QSizePolicy::Expanding);
      _text = new QLabel(QString("No devices detected!"));

      _image = new QLabel();
      _image->setPixmap(QPixmap(":/pixmaps/noDevice.png"));
      _layout->addItem(_top, 0, 0, 1, -1);
      _layout->addWidget(_text, 1, 0, 1, -1, Qt::AlignCenter);
      _layout->addWidget(_image, 2, 0, 1, -1, Qt::AlignCenter);
      _layout->addItem(_bottom, 3, 0, 1, -1);
      setLayout(_layout);
    } 

    private:
    QGridLayout* _layout;
    QSpacerItem* _top;
    QSpacerItem* _bottom;
    QLabel* _text;
    QLabel* _image;

  };
};
#endif
