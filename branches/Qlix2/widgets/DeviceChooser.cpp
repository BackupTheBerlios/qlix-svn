#include "widgets/DeviceChooser.h"

DeviceChooser::DeviceChooser(QWidget* parent, 
                             MtpSubSystem* in_subsystem) :
                             QScrollArea(parent) 
{
  count_t count = in_subsystem->DeviceCount();
  _deviceButtons.resize(count);

  _chooserGroupBox = new QGroupBox("Choose a device");
  _chooserLayout = new QGridLayout();

  _chooserLayout->setColumnMinimumWidth(0, 160);
    _chooserLayout->setVerticalSpacing(40);
  if (count > 1)
    _chooserLayout->setColumnMinimumWidth(1, 160);
  if (count > 2)
    _chooserLayout->setColumnMinimumWidth(2, 160);


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
    MtpDevice* temp = in_subsystem->GetDevice(0);
#else
    MtpDevice* temp = in_subsystem->GetDevice(i);
#endif

    _deviceButtons[i] = new DeviceButton(temp, this);
    _chooserLayout->addLayout(_deviceButtons[i], row, i%3, 1, 1);
  }

  for (i = 0; i < 3; i++)
  {
    QSpacerItem* temp = new QSpacerItem(10, 10, QSizePolicy::Minimum,
                                            QSizePolicy::Expanding);
    _chooserLayout->addItem(temp, 0, 1, -1, 1);  
    _chooserLayout->addItem(temp, 0, 3, -1, 1);  
  }

  _chooserGroupBox->setLayout(_chooserLayout);
  QScrollArea::setWidget(_chooserGroupBox);
  QScrollArea::setWidgetResizable(true);
  setupConnections();
}

void DeviceChooser::ExclusivelySelected(DeviceButton* selected)
{
  for (count_t i = 0; i < _deviceButtons.size(); i++)
  {
    if (_deviceButtons[i] != selected)
      _deviceButtons[i]->RemoveCheck();
  }
}

void DeviceChooser::setupConnections()
{
  for (int i = 0; i < _deviceButtons.size(); i++)
    QObject::connect(_deviceButtons[i], SIGNAL(Checked(DeviceButton*) ), this, SLOT(ExclusivelySelected(DeviceButton* )));
}
