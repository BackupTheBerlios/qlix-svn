#include "widgets/DeviceButton.h"

DeviceButton::DeviceButton (MtpDevice* in_device, QWidget* parent)
{
  _device = in_device;

  QString name = QString::fromUtf8(_device->name());
  _checkBox = new QCheckBox("Connect on startup");
  _button = new QToolButton();

  _button->setText(name);
  _button->setIcon(QIcon(":/pixmaps/miscDev.png"));
  _button->setMinimumSize(QSize(160,160));
  _button->setMaximumSize(QSize(160,160));
  _button->setIconSize(QSize(128,128));
  _button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  QSpacerItem* temp = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                            QSizePolicy::Expanding);

  addItem(temp);

  addWidget(_button);
  addWidget(_checkBox);

  temp = new QSpacerItem(10, 10, QSizePolicy::Maximum,
                                            QSizePolicy::Expanding);
  addItem(temp);


  setupConnections();
}

void DeviceButton::RemoveCheck()
{
  _checkBox->setCheckState(Qt::Unchecked);
}

//we can do better than this..
void DeviceButton::setupConnections()
{
  connect(_checkBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged (int)) );
}

void DeviceButton::stateChanged(int in)
{
  if (in == Qt::Checked)
    emit Checked(this);
}
