#include "MtpDevice.h"
/**
 * Creates a MtpDevice
 * @param in_device The raw LIBMtp device
 * @return Returns the requested device
 */
MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* in_device) 
{
  _device = in_device;
   _name = LIBMTP_Get_Friendlyname(_device);

  _serialNumber = LIBMTP_Get_Serialnumber(_device);
  _version = LIBMTP_Get_Deviceversion(_device);
  _syncPartner= LIBMTP_Get_Syncpartner(_device);
  _modelName = LIBMTP_Get_Modelname(_device);

  uint8_t max;
  uint8_t cur;
  int ret = LIBMTP_Get_Batterylevel(_device, &max, &cur); 

  if (ret != 0)
    _batteryLevelSupport = false; 

  _maxBatteryLevel     = max;
  _currentBatteryLevel = cur;
  ret = 0;


/* not the correct format..
  uint16_t count = 0;
  uint16_t* types;
  ret = LIBMTP_Get_Supported_Filetypes(_device, &types, &count);
  if (ret == 0)
  {
    for (count_t i =0; i < count; i++)
    {
      //char* s( (wchar_t*)types[i]);
      _supportedFileTypes.push_back(s);
    }
  }
  if (types)
    free(types);
    */
}

/**
 * Releases the wrapped device  
 */

MtpDevice::~MtpDevice() 
{
  if(_device)
  {
    LIBMTP_Release_Device(_device);
    _device = NULL;
  }

  if (_name)
  {
    delete _name;
    _name = NULL;
  }

  if (_serialNumber)
  {
    delete _serialNumber;
    _serialNumber = NULL;
  }

  if (_version)
  {
    delete _version;
    _version = NULL;
  }

  if (_syncPartner)
  {
    delete _syncPartner;
    _syncPartner = NULL;
  }

  if (_modelName)
  {
    delete _modelName;
    _modelName = NULL;
  }
}

/**
 * @return Returns the name of the device as a UTF8 string
 */
char* MtpDevice::name() const
{
  return _name;
}

/**
 * @return Returns the device serial number as a UTF8 string
 */
char* MtpDevice::serialNumber() const
{
  return _serialNumber;
}

/**
 * @return Returns the version of the device as a UTF8 string
 */
char* MtpDevice::version() const
{
  return _version;
}

/**
 * @return Returns the sync partner of the device as a UTF8 string
 */
char* MtpDevice::syncPartner() const
{
  return _syncPartner;
}

/**
 * @return Returns the model name of the device as a UTF8 string
 */
char* MtpDevice::modelName() const
{
  return _modelName;
}

/**
 * @return Returns whether retreiveing the battery level caused problems
 */
bool MtpDevice::BatterLevelSupport() const
{
  return _batteryLevelSupport;
}

/**
 * Returns the battery level in the form of a percentage;
 * @return Returns the percentage of charge the battery holds
 */
float MtpDevice::BatterLevel() const 
{
  float ret = (float) ((float)_maxBatteryLevel / (float)_currentBatteryLevel);
  return ret;
}
