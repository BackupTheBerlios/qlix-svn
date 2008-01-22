#include "widgets/DeviceChooser.h"

DeviceChooser::DeviceChooser(QWidget* parent)
{
  createNoDeviceWidget();
  initialize();
}


/* 
 * Initializes a button for each device detected, or displays the No devices detected scree
 */
void DeviceChooser::initialize()
{
  _chooserGroupBox = new QGroupBox("Choose a device");
  _chooserLayout = new QGridLayout(_chooserGroupBox);
  _chooserLayout->setVerticalSpacing(40);
  _chooserLayout->setColumnMinimumWidth(0, 160);

  QScrollArea::setWidgetResizable(true);
  showNoDeviceWidget();
  return;
}

/*

  count_t i; 
  count_t row =0; 

#ifdef MULTIPLE_DEVICES
    if (count == 1)
      count = 7;
  _deviceButtons.resize(count);
#endif

  for(i = 0; i < count; i++)
  {
    if( i != 0 && i %3 == 0)
      row++;
    qDebug() << "Detected %d devices" << count << " Pass: " << i;
#ifdef MULTIPLE_DEVICES
    MtpDevice* temp = _subsystem->GetDevice(0);
#else
    MtpDevice* temp = _subsystem->GetDevice(i);
#endif

    _deviceButtons[i] = new DeviceButton(temp);
    _chooserLayout->addLayout(_deviceButtons[i], row, i%3, 1, 1);
  }

  QScrollArea::setWidget(_chooserGroupBox);
  QScrollArea::setWidgetResizable(true);

#ifdef MULTIPLE_DEVICES
  if (count > 0)
  {
    _subsystem->GetDevice(0)->CreateObjectStructure();
    _subsystem->GetDevice(0)->ClearObjectMappings();
    _subsystem->GetDevice(0)->CreateObjectStructure();
  }
#endif
  setupConnections();
}

*/

/**
 * This slot iterates over all buttons and unchecks those that are not equal
 * to selected
 * @param selected the exclusively selected DeviceButton
 */
void DeviceChooser::ExclusivelySelected(DeviceButton* selected)
{
  for (int i = 0; i < _deviceButtons.size(); i++)
  {
    if (_deviceButtons[i] != selected)
      _deviceButtons[i]->RemoveCheck();
  }
}

void DeviceChooser::Reinitialize()
{
  return;
  /*
  if (_deviceButtons.size() > 0 && (layout() == _noDeviceLayout) )
  {
      delete layout();
      _noDeviceLayout = NULL;
      QScrollArea::setLayout(_chooserLayout);
      createNoDeviceWidget();
  }
  else
    QScrollArea::setLayout(_noDeviceLayout);
  update();
  */
}

/*
 * Adds a DeviceButton based on the passed device to this container widget
 * @param in_device the device to add a button for
 */
void DeviceChooser::AddDevice(QMtpDevice* in_device)
{
  _devices.push_back(in_device);
  DeviceButton* newDevice = new DeviceButton(in_device);
  _deviceButtons.push_back(newDevice);
  count_t row;
  count_t col;

  row = ((_deviceButtons.size() -1) / 3);

  col = (_deviceButtons.size() -1) % 3;

  _chooserLayout->addLayout(newDevice, row, col, 1, 1);

  if (_deviceButtons.size() > 1)
    _chooserLayout->setColumnMinimumWidth(1, 160);
  if (_deviceButtons.size() > 2)
    _chooserLayout->setColumnMinimumWidth(2, 160);
#ifdef QLIX_DEBUG
  qDebug() << "Added a new device!";
#endif
  if (_devices.size() == 1)
    QScrollArea::setWidget(_chooserGroupBox);
  QScrollArea::setWidgetResizable(true);
}



void DeviceChooser::createNoDeviceWidget()
{
  _noDeviceLayout = new NoDevice();
}

void DeviceChooser::showNoDeviceWidget()
{
  QScrollArea::setWidget(_noDeviceLayout);
//  _chooserLayout->addWidget(_noDeviceLayout, 0, 0, -1, -1);
}
void DeviceChooser::hideNoDeviceWidget()
{
//  removeWidget(_noDeviceLayout);
//  _noDeviceLayout->hide();
}

/*
 * Uncommented because of development
 */
void DeviceChooser::deviceCountChanged()
{ }

/*
 * Connects each button's check box to the ExclusivelySelected() slot 
 */
void DeviceChooser::setupConnections()
{
  for (int i = 0; i < _deviceButtons.size(); i++)
    QObject::connect(_deviceButtons[i], SIGNAL(Checked(DeviceButton*) ),
                     this, SLOT(ExclusivelySelected(DeviceButton* )));
}


