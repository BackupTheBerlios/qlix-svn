#include "MtpSubSystem.h"
/**
 * Does nothing for now..
*/
MtpSubSystem::MtpSubSystem()
{
}


MtpSubSystem::~MtpSubSystem()
{
  ReleaseDevices();
}

/**
  * Initializes the MTP subsytem and retrive device list
  */
void MtpSubSystem::Initialize()
{
  LIBMTP_error_number_t tempNum;
  LIBMTP_Init();
  tempNum = LIBMTP_Get_Connected_Devices(&_deviceList);
  LIBMTP_mtpdevice_t* _dlist = _deviceList;
  while(_dlist)
  {
    MtpDevice* dev = new MtpDevice(_dlist);
    _devList.push_back(dev);
    _dlist = _dlist->next;
  }

  cout << " sub system Detected " << _devList.size() << " devices" << endl;
}

/**
 * Releases all MTP devices for a gracefull shutdown
 */
void MtpSubSystem::ReleaseDevices()
{
  for (count_t i = 0; i < _devList.size(); i++)
  {
    if (_devList[i])
    {
      delete _devList[i];
      _devList[i] = NULL;
    }
  }
  _deviceList = NULL;
}

/**
 * @return Returns the number of discovered devices
 */
count_t MtpSubSystem::DeviceCount() const
{
  return _devList.size();
}

/**
 * Returns the requested device by index ID
 * @param idx the index of the device to reteive
 * @return Returns the requested device
 */
MtpDevice* MtpSubSystem::GetDevice(count_t idx)
{
  if (idx >= _devList.size())
    return NULL;
  else
    return _devList[idx];
}
