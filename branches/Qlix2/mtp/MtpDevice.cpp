//TODO Add callback feature
//TODO Add sample data feature
//TODO Add supported file types
#include "MtpDevice.h"
/**
 * Creates a MtpDevice
 * @param in_device The raw LIBMtp device
 * @return Returns the requested device
 */
MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* in_device) 
{
  _device = in_device;
  _progressFunc= NULL;
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
 * @return Returns whether retreiveing the battery level failed
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

/**
 * Gets the device's error stack and runs over each error to store its text 
 * internally and then clears the device's error stack
 */
void MtpDevice::processErrorStack()
{
  if (!_device)
    return;
  LIBMTP_error_t* es = LIBMTP_Get_Errorstack(_device);
  while (es)
  {
    string s(es->error_text);
    _errorStack.push_back(s);
    es = es->next;
  }

  LIBMTP_Clear_Errorstack(_device);
}

/**
 * Sets the callback progress function for operations with the MTP device
 * @param in_func the callback function to invoke during MTP operations
 */
void MtpDevice::SetProgressFunction(LIBMTP_progressfunc_t in_func)
{
  _progressFunc = in_func;
}

/**
 * Retrieves Tracks, Files and Albums and Folders form the devices and creates
 * a tree, storing each object's ID in _objectMap
 */
void MtpDevice::CreateObjectStructure()
{
  if (!_device)
    return;
  LIBMTP_file_t* fileRoot = LIBMTP_Get_Filelisting(_device);
  while (fileRoot)
  {
    count_t size = _objectMap.size();
    MTP::File* currentFile = new MTP::File(fileRoot, NULL);
    _objectMap[currentFile->GetID()] = currentFile; 

    //assert that we increase in size by one (and thus haven't overwritten 
    //a pervious entry
    assert(_objectMap.size() == size+1);
    fileRoot = fileRoot->next;
  }

  LIBMTP_track_t* trackRoot = LIBMTP_Get_Tracklisting(_device);
  while (trackRoot)
  {
    count_t size = _objectMap.size();
    MTP::Track* currentTrack = new MTP::Track(trackRoot);
    MTP::GenericObject* temp = _objectMap[currentTrack->GetID()];
    MTP::File* temp2 = (MTP::File*) temp;
    _objectMap[currentTrack->GetID()] = currentTrack; 

    //assert that we didn't overwrite a pervious entry
    if(_objectMap.size() != size+1){
    {
      std::cout <<"objectsize: " << _objectMap.size() << "should be: " << size+1 << std::endl;
      std::cout<< "object id is: " << currentTrack->GetID();
      assert(false);
    }
    trackRoot = trackRoot->next;
  }

  LIBMTP_album_t* albumRoot= LIBMTP_Get_Album_List(_device);
  while (albumRoot)
  {
    count_t size = _objectMap.size();
    MTP::Album* currentAlbum = new MTP::Album(albumRoot);
    _objectMap[currentAlbum->GetID()] = currentAlbum; 

    //assert that we didn't overwrite a pervious entry
    assert(_objectMap.size() == size+1);
    albumRoot = albumRoot->next;
  }

  LIBMTP_folder_t* folderRoot= LIBMTP_Get_Folder_List(_device);
  LIBMTP_folder_t* sibling;
  }
  /* tobe written
  while (folderRoot)
  {
    sibling = folderRoot->sibling;
    while (sibling)
    {
      count_t size = _objectMap.size();
      MTP::Folder* currentFolder = new MTP::Folder(sibling);
      _objectMap[currentFolder->GetID()] = currentFolder; 

      //assert that we didn't overwrite a pervious entry
      assert(_objectMap.size() == size+1);
      folderRoot = folderRoot->sibling;
    }

    while (folderRoot)
    {
      count_t size = _objectMap.size();
      MTP::Folder* currentFolder = new MTP::Folder(folderRoot);
      _objectMap[currentFolder->GetID()] = currentFolder; 

      //assert that we didn't overwrite a pervious entry
      assert(_objectMap.size() == size+1);
      folderRoot = folderRoot->child;
    }
  }
  //Now iterate over every album and folder adding their respective children 
  //by using the map 
  */
}
