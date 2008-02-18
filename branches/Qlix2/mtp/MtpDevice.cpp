//TODO Improve error handling / Reporting (once theres an error console)
//TODO Should raw object references returns be of const types? 
//     No this is a bad idea as sending files updates the file id we discover

#include "MtpDevice.h"
#undef QLIX_DEBUG
//#define SIMULATE_TRANSFERS

/**
 * Creates a MtpDevice
 * @param in_device The raw LIBMtp device
 * @return Returns the requested device
 */
MtpDevice::MtpDevice(LIBMTP_mtpdevice_t* in_device) 
{
  _device = in_device;
  _serialNumber = LIBMTP_Get_Serialnumber(_device);
}

/**
 * Deletes allocated objects
 */
MtpDevice::~MtpDevice() 
{
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
 * Release the wrapped device
 */
void MtpDevice::ReleaseDevice()
{
  if(_device)
  {
    LIBMTP_Release_Device(_device);
    _device = NULL;
  }
}




/**
 * Initializes the MtpDevice and creates an internal tree structure for all
 * MTP objects
 */
void MtpDevice::Initialize()
{
  if (!_device)
    return;
//  _progressFunc= NULL;
   _name = LIBMTP_Get_Friendlyname(_device);

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
  UpdateSpaceInformation();
}

/**
 * Returns the number of files on the device
 */
count_t MtpDevice::FileCount() const
{ return _files.size(); }

void MtpDevice::FreeSpace(uint64_t* total, uint64_t* free)
{
  *total = _totalSpace;
  *free = _freeSpace;
}

/**
 * Returns the number of folders found at the root level of the device
 */
count_t MtpDevice::RootFolderCount() const
{ return _rootFolders.size(); }

/**
 * Returns the number of files found at the root level of the device
 */
count_t MtpDevice::RootFileCount() const
{ return _rootFiles.size(); }

/**
 * Returns the Folder at the given index if it exists
 * @return the Folder at the given index or NULL if it is out of bounds
 */
MTP::Folder* MtpDevice::RootFolder(count_t idx) const
{ 
  if (idx > _rootFolders.size() )
    return NULL;
  return _rootFolders[idx];
}

/**
 * Returns the root File at the given index if it exists
 * @return the root File at the given index or NULL if it is out of bounds
 */
MTP::File* MtpDevice::RootFile(count_t idx) const
{ 
  if (idx > _rootFiles.size() )
    return NULL;
  return _rootFiles[idx];
}


/**
 * Returns the number of albums on the device
 */
count_t MtpDevice::AlbumCount() const
{ return _albums.size(); }

/**
 * Returns the number of playlists on the device
 */
count_t MtpDevice::PlaylistCount() const
{ return _playlists.size(); }



/**
 * Returns the File at the given index if it exists
 * @return the File at the given index or NULL if it is out of bounds
 */
MTP::File* MtpDevice::File(count_t idx) const
{ 
  if (idx > _files.size() )
    return NULL;
  return _files[idx];
}

/**
 * Returns the Album at the given index if it exists
 * @return the Album at the given index or NULL if it is out of bounds
 */
MTP::Album* MtpDevice::Album(count_t idx) const
{ 
  if (idx > _albums.size() )
    return NULL;
  return _albums[idx];
}

/**
 * Returns the Playlist at the given index if it exists
 * @return the Playlist at the given index or NULL if it is out of bounds
 */
MTP::Playlist* MtpDevice::Playlist(count_t idx) const
{ 
  if (idx > _playlists.size() )
    return NULL;
  return _playlists[idx];
}


/**
 * Retrieves the object to the specificed path
 * @param in_id the item id of the requested Mtp object
 * @param path the path to retreive to
 */
bool MtpDevice::Fetch(uint32_t in_id, char const * const path)
{
  if (!_device)
    return false;
//TODO get error log
  int ret= LIBMTP_Get_File_To_File(_device, in_id, path, 
                                   _progressFunc, _progressData);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  return true; 
}


/**
 * @return Returns the name of the device as a UTF8 string
 */
char const * const MtpDevice::Name() const
{
  return _name;
}

/**
 * @return Returns the device serial number as a UTF8 string
 * This is mostly used for quickly connecting to a default device
 */
char const * const MtpDevice::SerialNumber() const
{
  return _serialNumber;
}

/**
 * @return Returns the version of the device as a UTF8 string
 */
char const * const MtpDevice::Version() const
{
  return _version;
}

/**
 * @return Returns the sync partner of the device as a UTF8 string
 */
char const * const MtpDevice::SyncPartner() const
{
  return _syncPartner;
}

/**
 * @return Returns the model name of the device as a UTF8 string
 */
char const * const MtpDevice::ModelName() const
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
void MtpDevice::SetProgressFunction(LIBMTP_progressfunc_t in_func,
                                    const void* const in_data)
{
  _progressData = in_data;
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
    MTP::File* currentFile = new MTP::File(fileRoot);
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
    LIBMTP_filesampledata_t temp;
    LIBMTP_Get_Representative_Sample(_device, albumRoot->album_id, &temp); 
//    cout << "Discovred a sample of type: " << temp.filetype << endl;
//    cout << "Discovred a sample of height: " << temp.height << " of width: "<< temp.width << " size: " << temp.size << endl;

    count_t size = _objectMap.size();

    MTP::Album* currentAlbum = new MTP::Album(albumRoot, temp);
    currentAlbum->SetRowIndex( _albums.size());
    _albums.push_back(currentAlbum);

    MTP::GenericObject* previous = _objectMap[currentAlbum->ID()];
    _objectMap[currentAlbum->ID()] = currentAlbum; 

    //crosslink check
    if(_objectMap.size() != size+1)
    {
      assert(previous);
      _crossLinked.push_back(previous);
      _crossLinked.push_back( currentAlbum );
      crossLinkCount++;
    }

    albumRoot = albumRoot->next;
  }
#ifdef QLIX_DEBUG
  cerr << "Album cross link count: " << crossLinkCount << endl;
#endif

  LIBMTP_playlist_t* playlistRoot = LIBMTP_Get_Playlist_List(_device);
  crossLinkCount = 0;
  while(playlistRoot)
  {
    count_t size = _objectMap.size();

    MTP::Playlist* currentPlaylist = new MTP::Playlist(playlistRoot);
    _playlists.push_back(currentPlaylist);
    currentPlaylist->SetRowIndex(_playlists.size());

    MTP::GenericObject* previous = _objectMap[currentPlaylist->ID()];
    _objectMap[currentPlaylist->ID()] = currentPlaylist; 
    if (_objectMap.size() != size+1)
    {
      assert(previous);
      _crossLinked.push_back(previous);
      _crossLinked.push_back(currentPlaylist);
      crossLinkCount++;
    }
    playlistRoot = playlistRoot->next;
  }
  createFolderStructure(NULL);
  createFileStructure();
  createAlbumStructure();
  createPlaylistStructure();
  cout << "Crosslinked entries: " << _crossLinked.size() << endl;

#ifdef QLIX_DEBUG
  //dbgPrintSupportedFileTypes();
  //dbgPrintFolders(NULL, 0);
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
      temp->SetRowIndex(in_root->FolderCount());
    }
    else //else set the child's parent to NULL indicating its at the root
    {
      temp =  new MTP::Folder(folderRoot, NULL);
      //add to the root level folder
      _rootFolders.push_back(temp);
      temp->SetRowIndex(_rootFolders.size());
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
    temp->SetParent(parentFolder);
    cout << "Set " << parentFolder->Name() << " as parent of " << temp->Name() << endl;
  }
}

/**
 * Iterates over all the Albums and their tracks Each track is looked up in 
 * the object map and added to the Album's vector  of tracks
 */
void MtpDevice::createAlbumStructure()
{
  MTP::GenericObject* obj;
  MTP::Album* parentAlbum;
  for (count_t i =0; i < _albums.size(); i++)
  {
    parentAlbum = _albums[i];
    for (count_t j = 0; j < parentAlbum->TrackCount(); j++)
    {
      uint32_t track_id = parentAlbum->ChildTrackID(j);
      //sanity check
      count_t size = _objectMap.size();
      obj = _objectMap[track_id];
//      assert(_objectMap.size() == size+1); //because its crosslinked with tracks i guess
      if (obj->Type() != MtpTrack)
      {
        cerr << "Current track: " << track_id << "is crosslinked" << endl;
        continue;
      }
      parentAlbum->AddChildTrack((MTP::Track*) obj);
    }
    parentAlbum->SetInitialized();
  }
}


/**
 * Iterates over all the Playlists and their tracks. Each track is looked up in 
 * the object map and added to the Playlist's vector of tracks
 */
void MtpDevice::createPlaylistStructure()
{
  MTP::GenericObject* obj;
  MTP::Playlist* parentPlaylist;
  for (count_t i =0; i < _playlists.size(); i++)
  {
    parentPlaylist = _playlists[i];
    for (count_t j = 0; j < parentPlaylist->TrackCount(); j++)
    {
      uint32_t track_id = parentPlaylist->ChildTrackID(j);
      //sanity check
      count_t size = _objectMap.size();
      obj = _objectMap[track_id];
      assert(_objectMap.size() == size);
      if (obj->Type() != MtpTrack)
      {
        cerr << "Current track: " << track_id << "is crosslinked" << endl;
        continue;
      }
      parentPlaylist->AddChildTrack( (MTP::Track*) obj );
    }
    parentPlaylist->SetInitialized();
  }
}

bool MtpDevice::TransferTrack(const char* in_path, MTP::Track* track)
{
#ifndef SIMULATE_TRANSFERS
  int ret = LIBMTP_Send_Track_From_File(_device, in_path, track->RawTrack(),
                                        _progressFunc, _progressData, 
                                        track->RawTrack()->parent_id);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif

  //necessary due to stupid inheritence
  //TODO fix this?
  track->SetID(track->RawTrack()->item_id);
  cout << "Transfer succesfull, new id: " << track->ID() << endl;
  UpdateSpaceInformation();
  return true;
}


bool MtpDevice::TransferFile(const char* in_path, MTP::File* file)
{
  int ret = LIBMTP_Send_File_From_File(_device, in_path, file->RawFile(),
                                        _progressFunc, _progressData, 
                                        file->RawFile()->parent_id);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  UpdateSpaceInformation();
  return true;
}

/**
 * This function updates the space usage information, it should be called at
 * every function call that could potentially change the space usage of the 
 * device
 * @return true if successfully retreived, false otherwise
 */
bool MtpDevice::UpdateSpaceInformation()
{
  if (!_device)
    return false;
  int ret = LIBMTP_Get_Storage(_device, LIBMTP_STORAGE_SORTBY_NOTSORTED);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  (_totalSpace) = _device->storage->MaxCapacity;
  (_freeSpace)  = _device->storage->FreeSpaceInBytes;
  return true;
}

/**
 * This function creates a new album on the device, using the information from
 * the track that is passed as a param. This function assumes that this 
 * album's name is unique on the device. 
 * The new album will be added to the album's list, be sure to notify any
 * models before hand
 *
 * @param in_track the track that is used as a template for the album's name,
 *        artist, and genre fields.
 * @param out_album the newly allocated MTP::Album, if the operation fails
 *        this value is NULL
 * @return true if successfull false otherwise
 */
bool MtpDevice::CreateNewAlbum(MTP::Track* in_track, MTP::Album** out_album)
{
  LIBMTP_album_t* newAlbum = LIBMTP_new_album_t();
  newAlbum->name = strdup(in_track->AlbumName());
  newAlbum->artist = strdup(in_track->ArtistName());
  newAlbum->genre = strdup(in_track->Genre());
  newAlbum->tracks  = NULL;
  cerr << "Created new album with name: " << newAlbum->name << endl;
  cerr << "Created new album with artist: " << newAlbum->artist << endl;
  cerr << "Created new album with genre: " << newAlbum->genre << endl;
//  *(newAlbum->tracks) = in_track->ID();
//  cout << "Set the album's first track to: " << *(newAlbum->tracks) << endl;
  newAlbum->no_tracks = 0;
  newAlbum->next = NULL;
#ifndef SIMULATE_TRANSFERS
  int ret =  LIBMTP_Create_New_Album(_device, newAlbum, 0);
  if (ret != 0)
  {
    (*out_album) = NULL;
    processErrorStack();
    return false;
  }
#endif
  UpdateSpaceInformation();
  LIBMTP_filesampledata_t sample;
  sample.size = 0;
  sample.data = NULL;
  (*out_album) = new MTP::Album(newAlbum, sample);
  return true;
}

/**
 * This function updates the representative sample of album on the device 
 * @return true if the operation succeeded, false otherwise.
 */
bool MtpDevice::UpdateAlbumArt(MTP::Album* in_album, 
                               LIBMTP_filesampledata_t* in_sample)
{
  int ret = LIBMTP_Send_Representative_Sample(_device,
                                              in_album->ID(), in_sample);
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
  in_album->SetCover(in_sample);
  return true;
}

/**
 * This function retreives the default JPEG sample parameters from the device
 * @return a preallocated LIBMTP_filesampledata_t* with sane values it is up to
 *         the caller to call LIBMTP_delete_object to free up the memory
 */
//TODO is this safe when mtp is not initialized?
LIBMTP_filesampledata_t* MtpDevice::DefaultJPEGSample()
{
  LIBMTP_filesampledata_t* sample;
  int ret = LIBMTP_Get_Representative_Sample_Format(_device, 
                                                    LIBMTP_FILETYPE_JPEG,
                                                    &sample);
  if (ret != 0)
  { 
    processErrorStack();
    return NULL;
  }
  return sample;
}

/**
 * This function adds an album to the Album list
 */
void MtpDevice::AddAlbum(MTP::Album* in_album)
{
  in_album->SetRowIndex(_albums.size());
  _albums.push_back(in_album);
}

/**
 * This function adds the passed track to the album on the device by first
 * adding it to the raw track, syncing to the device and then adds the track
 * to the wrapper track. 
 * @param in_track the track that is to be added to the parent album
 * @param in_album the parent album for this track 
 * @return true if successfull false otherwise
 */
bool MtpDevice::AddTrackToAlbum(MTP::Track* in_track, MTP::Album* in_album)
{
  in_album->SetInitialized();
  in_album->AddTrackToRawAlbum(in_track);
#ifndef SIMULATE_TRANSFERS
  int ret = LIBMTP_Update_Album(_device, in_album->RawAlbum());
  if (ret != 0)
  {
    processErrorStack();
    return false;
  }
#endif
  return true;
}

