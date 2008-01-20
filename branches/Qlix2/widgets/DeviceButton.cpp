#include "widgets/DeviceButton.h"

/**
 * Creates a DeviceButton object
 * @param in_device The device that this button represents
 * @param parent The parent widget
 * @return Returns a DeviceButton object
 */
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

  in_device->CreateObjectStructure();
  setupConnections();
}

/**
 * Removes checked state of the check box if it is checked
 */
void DeviceButton::RemoveCheck()
{
  if (_checkBox->checkState() == Qt::Checked)
    _checkBox->setCheckState(Qt::Unchecked);
}

/**
 * Setup up the widgets internal connections
 * See also stateChanged()
 */
void DeviceButton::setupConnections()
{
  connect(_checkBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged (int)) );
}

/**
 * Private slot that is called when the checkbox is checked 
 * See also setupConnections()
 */
void DeviceButton::stateChanged(int in)
{
  if (in == Qt::Checked)
    emit Checked(this);
}
