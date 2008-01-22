//TODO Add callback feature
//TODO Add/Test sample data feature
//TODO Add playlist feature
//TODO Track structure creation is faulty
#include "MtpDevice.h"
/**
 * Creates a MtpDevice
 * @param in_device The raw LIBMtp device
 * @return Returns the requested device
 */
MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* in_device) 
{
  _device = in_device;

}

/**
 * Initializes the MtpDevice and creates an internal tree structure for all
 * MTP objects
 */
void MtpDevice::Initialize()
{
  if (!_device)
    return;
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


  uint16_t count = 0;
  uint16_t* types;
  ret = LIBMTP_Get_Supported_Filetypes(_device, &types, &count);
  if (ret == 0)
  {
    for (count_t i =0; i < count; i++)
    {
      char const* str;
      str = LIBMTP_Get_Filetype_Description( (LIBMTP_filetype_t)types[i] );
      string s(str);
      _supportedFileTypes.push_back(s);
    }
  }
  if (types)
    free(types);
  createObjectStructure();
}

/**
 * Returns the number of files on the device
 */
count_t MtpDevice::FileCount()
{ return _files.size(); }

/**
 * Returns the File at the given index if it exists
 * @return the File at the given index or NULL if it is out of bounds
 */
MTP::File* MtpDevice::File(count_t idx)
{ 
  if (idx > _files.size() )
    return NULL;
  return _files[idx];
}

/**
 * Retreives the object to the specificed path
 * @param in_id the item id of the requested Mtp object
 * @param path the path to retreive to
 */
void MtpDevice::Retreive(count_t in_id, char const * const path)
{
  if (!_device)
    return;
//TODO get error log
  LIBMTP_Get_File_To_File(_device, in_id, path, NULL, NULL);
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

  ClearObjectMappings();
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
void MtpDevice::createObjectStructure()
{
  if (!_device)
    return;
  LIBMTP_file_t* fileRoot = LIBMTP_Get_Filelisting(_device);
  while (fileRoot)
  {
    //we should be careful to check the map's size before we check for 
    //crosslinks
    count_t size = _objectMap.size();
    LIBMTP_filesampledata_t temp;
    LIBMTP_Get_Representative_Sample(_device, fileRoot->item_id, &temp); 

    MTP::File* currentFile = new MTP::File(fileRoot, temp);
    _files.push_back(currentFile);
    MTP::GenericObject* previous = _objectMap[currentFile->ID()];
    _objectMap[currentFile->ID()] = currentFile; 

    //crosslink check
    if(_objectMap.size() != size+1)
    {
      assert(previous);
      _crossLinked.push_back(previous);
      _crossLinked.push_back( currentFile);
    }
    fileRoot = fileRoot->next;
  }

  LIBMTP_track_t* trackRoot = LIBMTP_Get_Tracklisting(_device);
  while (trackRoot)
  {
    count_t size = _objectMap.size();
    MTP::Track* currentTrack = new MTP::Track(trackRoot);
    _tracks.push_back(currentTrack);
    MTP::GenericObject* previous = _objectMap[currentTrack->ID()];
    _objectMap[currentTrack->ID()] = currentTrack; 

    //crosslink check
    if(_objectMap.size() != size+1)
    {
      assert(previous);
      _crossLinked.push_back(previous);
      _crossLinked.push_back( currentTrack );
    }
    trackRoot = trackRoot->next;
  }

  LIBMTP_album_t* albumRoot= LIBMTP_Get_Album_List(_device);
  count_t crossLinkCount = 0;
  while (albumRoot)
  {
    count_t size = _objectMap.size();
    MTP::Album* currentAlbum = new MTP::Album(albumRoot);
    _objectMap[currentAlbum->ID()] = currentAlbum; 
    MTP::GenericObject* previous = _objectMap[currentAlbum->ID()];

    //crosslink check
    //coun
    if(_objectMap.size() != size+1)
    {
      assert(previous);
      _crossLinked.push_back(previous);
      _crossLinked.push_back( currentAlbum );
      crossLinkCount++;
    }

    albumRoot = albumRoot->next;
  }
  cerr << "Album cross link count: " << crossLinkCount << endl;

  createFolderStructure(NULL);
  createFileStructure();
  createTrackStructure();
  cout << "Crosslinked entries: " << _crossLinked.size() << endl;
#ifdef QLIX_DEBUG
  dbgPrintSupportedFileTypes();
  dbgPrintFolders(NULL, 0);
#endif
}

/*
 * Returns the raw device that this object abstracts over
 * @return the raw device
 */
LIBMTP_mtpdevice_t* MtpDevice::RawDevice() const
{
  return _device;
}

/**
 * Recursively builds the directory structure
 */
void MtpDevice::createFolderStructure(MTP::Folder* in_root)
{
  if (!_device)
    return;
  vector<MTP::Folder*> curLevelFolders;
  LIBMTP_folder_t* folderRoot;
  if (!in_root)
    folderRoot= LIBMTP_Get_Folder_List(_device);
  else
    folderRoot = in_root->RawFolder()->child;

  while (folderRoot)
  {
    count_t size = _objectMap.size();
    //if there is a parent, set the new folder's parent. And add to the 
    //parent's childlist
    MTP::Folder* temp;
    if(in_root)
    {
      temp =  new MTP::Folder(folderRoot, in_root);
      in_root->AddChildFolder(temp);
    }
    else //else set the child's parent to NULL indicating its at the root
    {
      temp =  new MTP::Folder(folderRoot, NULL);
      //add to the root level folder
      _rootFolders.push_back(temp);
    }

    //add this folder to the list of folders at this level
    curLevelFolders.push_back(temp);
    
    //previous is used if there is a crosslinked entry
    MTP::GenericObject* previous = _objectMap[temp->ID()];
    _objectMap[temp->ID()] = temp; 

    //crosslink check
    if(_objectMap.size() != size+1)
    {
      assert(previous);
      _crossLinked.push_back(previous);
      _crossLinked.push_back(temp);
    }
    folderRoot = folderRoot->sibling;
  }
  for (count_t i =0; i < curLevelFolders.size(); i++)
      createFolderStructure(curLevelFolders[i]);
}

/**
 * Prints the file types supported by this device
 */
void MtpDevice::dbgPrintSupportedFileTypes()
{
  cout << "Supported file types: ";
  if (_supportedFileTypes.size() == 0)
    cout << "none!";
  cout << endl;

  for (count_t i =0; i < _supportedFileTypes.size(); i++)
    cout << _supportedFileTypes[i] << endl;
}

/**
 * Recursively prints the folders discovered
 * @param root the current level's root folder
 * @level the current depth of the traversal used for alignment
 */
void MtpDevice::dbgPrintFolders(MTP::Folder* root, count_t level)
{
  if (root == NULL)
  {
    for (count_t i =0; i < _rootFolders.size(); i++)
    {
      cout << _rootFolders[i]->Name() << endl;
      dbgPrintFolders(_rootFolders[i], 1);
    }
    return;
  }

  for (count_t i = 0; i < root->FolderCount(); i++)
  {
    for (count_t j = 0; j < level; j++)
      cout << "  ";
    MTP::Folder* temp = root->SubFolder(i); 
    cout << temp->Name() << endl;
    dbgPrintFolders(temp, level+1);
  }
}
/**
 * Frees all memory used by instantiated objects
 */
void MtpDevice::ClearObjectMappings()
{
  for (count_t i =0; i < _crossLinked.size(); i++)
  {
    //used for sanity checking make sure that when we delete files we delete
    //from the crosslinked list as well
    count_t size = _objectMap.size();
    MTP::GenericObject* current = _crossLinked[i];
    MTP::GenericObject* mappedObject = _objectMap[current->ID()];
    //sanity make sure the map hasn't increased in size..
    assert(_objectMap.size() == size);

    if (mappedObject != current && current)
      delete current;
  }

  map<uint32_t, MTP::GenericObject*>::iterator iter;
  for (iter = _objectMap.begin(); iter != _objectMap.end(); iter++)
  {
    if(iter->second); //make sure its not NULL..
      delete iter->second;
  }
  _objectMap.clear();
  _crossLinked.clear();
  _rootFolders.clear();
  _rootFiles.clear();
  _files.clear();
  _tracks.clear();
  _albums.clear();
  _playlists.clear();
}

/**
 * Iterates over all the files and adds them to their parent folder
 */
void MtpDevice::createFileStructure()
{
  MTP::GenericObject* obj;
  MTP::Folder* parentFolder;
  for (count_t i =0; i < _files.size(); i++)
  {
    MTP::File* temp = _files[i];
    //sanity check
    count_t size = _objectMap.size();
    if (temp->ParentID() == 0)
    {
      _rootFiles.push_back(temp);
      continue;
    }
    obj =  _objectMap[temp->ParentID()];
    assert(_objectMap.size() == size);
    if (obj->Type() != MtpFolder)
    {
      cerr << "Serious crosslink problem: could not get parent folder: " << temp->ParentID() << endl;
      continue;
    }
    parentFolder = (MTP::Folder*) obj;
    parentFolder->AddChildFile(temp);
  }
}

/**
 * Iterates over all the tracks and adds them to their parent album 
 */
void MtpDevice::createTrackStructure()
{
  MTP::GenericObject* obj;
  MTP::Album* parentAlbum;
  for (count_t i =0; i < _tracks.size(); i++)
  {
    MTP::Track* temp = _tracks[i];
    //sanity check
    assert(temp->ParentID() != 0);
    count_t size = _objectMap.size();
    obj =  _objectMap[temp->ParentID()];
    assert(_objectMap.size() == size);
    if (obj->Type() != MtpAlbum)
    {
      cerr << "Serious crosslink problem: could not get parent album: " << temp->ParentID() << endl;
      continue;
    }
    parentAlbum = (MTP::Album*) obj;
    parentAlbum->AddChildTrack(temp);
  }
}
