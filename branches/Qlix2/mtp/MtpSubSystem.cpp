//TODO better error handling
#include "MtpSubSystem.h"
/**
 * Does nothing for now..
*/
MtpSubSystem::MtpSubSystem()
{
  LIBMTP_Init();
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
#ifndef MULTIPLE_DEVICES
  tempNum = LIBMTP_Get_Connected_Devices(&_deviceList);
  LIBMTP_mtpdevice_t* _dlist = _deviceList;
  while(_dlist)
  {
    MtpDevice* dev = new MtpDevice(_dlist);
    _devList.push_back(dev);
    _dlist = _dlist->next;
  }
#else
  LIBMTP_mtpdevice_t* first = LIBMTP_Get_First_Device();
  if (first == NULL)
    return;
  for (int i =0; i < 7; i++)
  {
    MtpDevice* dev = new MtpDevice(first);
    _devList.push_back(dev);
  }
#endif
  //cout << " sub system Detected " << _devList.size() << " devices" << endl;
}

/**
 * Releases all MTP devices for a gracefull shutdown
 */
void MtpSubSystem::ReleaseDevices()
{
#ifndef MULTIPLE_DEVICES
  for (count_t i = 0; i < _devList.size(); i++)
  {
    if (_devList[i])
    {
      _devList[i]->ReleaseDevice();
      delete _devList[i];
      _devList[i] = NULL;
    }
  }
  _devList.clear();
  _deviceList = NULL;
#else
  for (count_t i=0; i < _devList.size(); i++)
  {
    if (i == 0)
      _devList[0]->ReleaseDevice();
    delete _devList[i];
    _devList[i] = NULL;
  }
  _devList.clear();
#endif
}

/**
 * @return the number of discovered devices
 */
count_t MtpSubSystem::DeviceCount() const
{
  return _devList.size();
}

/**
 * This function has been scrapped as we need to use HAL/DBUS
 */
count_t MtpSubSystem::RawDeviceCount (MtpDeviceVector* connected, 
                                      MtpDeviceVector* disconnected,
                                      MtpDeviceVector* newDevice)
{
  return 0;
  /*
  LIBMTP_mtpdevice_t* list; 
  LIBMTP_error_number_t tempNum;
  tempNum = LIBMTP_Get_Connected_Devices(&list);
  count_t newDeviceCount = LIBMTP_Number_Devices_In_List(list);

  vector <LIBMTP_mtpdevice_t*> currentDevList;
  vector <MtpDevice*> oldDevList;
  oldDevList.swap(_devList);

  //iterate over the list of previously found devices and figure out who has 
  //been disconnected
  while(list)
  {
    currentDevList.push_back(list);
    list = list->next;
  }
  assert (currentDevList.size() == newDeviceCount);

  for (count_t i =0; i < oldDevList.size(); i++)
  {
    bool seen = false;

    for (count_t j = 0; j < newDeviceCount; j++)
    {
      if (oldDevList[i]->RawDevice() == currentDevList[j] )
        seen = true;
    }
    if (seen)
    {
      connected->push_back(oldDevList[i]);
      _devList.push_back(oldDevList[i]);
    }
    if (!seen)
      disconnected->push_back(oldDevList[i]);
  }

  for (count_t i =0; i< newDeviceCount; i++)
  {
    bool seen = false;
    MtpDevice* seenDevice;
    for (count_t j =0; j < oldDevList.size(); j++)
    {
      if (currentDevList[i] == oldDevList[j]->RawDevice()) 
      {
        assert(!seen);
        seen = true;
        seenDevice = oldDevList[j];
      }
    }
    if (!seen)
    {
      MtpDevice* dev = new MtpDevice(currentDevList[i]);
      newDevice->push_back(dev);
      _devList.push_back(dev);
    }
  }
  assert(_devList.size() == newDevice->size() + connected->size());
  assert(_devList.size() == newDeviceCount);
  return newDeviceCount;
  */
}

/**
 * Returns the requested device by index ID
 * @param idx the index of the device to reteive
 * @return Returns the requested device
 */
MtpDevice* MtpSubSystem::Device(count_t idx)
{
  if (idx >= _devList.size())
    return NULL;
  else
    return _devList[idx];
}
