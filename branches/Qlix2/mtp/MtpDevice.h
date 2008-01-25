#ifndef __MTPDEVICE__
#define __MTPDEVICE__
#include <libmtp.h>
#include <string>
#include <map>
#include <iostream>
#include "types.h"
#include "mtp/MtpObject.h"
using namespace std;

/** 
 * @class MtpDevice is a wrapper class around libmtp's device module
*/
class MtpDevice
{
public:
  MtpDevice(LIBMTP_mtpdevice_t* in_);
  ~MtpDevice();

  void Initialize();

  char const * const Name() const;
  char const * const SerialNumber() const;
  char const * const Version() const;
  char const * const SyncPartner() const;
  char const * const ModelName() const;

  bool BatterLevelSupport() const;
  float BatterLevel() const;

  void SetProgressFunction(LIBMTP_progressfunc_t );
  void ClearObjectMappings();
  void Retreive(count_t , char const * const );

  count_t FileCount() const;
  count_t AlbumCount() const;
  count_t PlaylistCount() const;

  MTP::File* File(count_t idx) const;
  MTP::Album* Album(count_t idx) const;
  MTP::Playlist* Playlist(count_t idx) const;

  count_t RootFolderCount() const;
  count_t RootFileCount() const;

  MTP::Folder* RootFolder(count_t idx) const;
  MTP::File* RootFile(count_t idx) const;

  void ReleaseDevice();

  LIBMTP_mtpdevice_t* RawDevice() const;
private:
  LIBMTP_mtpdevice_t* _device;
  char* _name;
  char* _serialNumber;
  char* _version;
  char* _syncPartner;
  char* _modelName;
  count_t _maxBatteryLevel;
  count_t _currentBatteryLevel;
  bool _batteryLevelSupport;
  LIBMTP_progressfunc_t _progressFunc;

  map<uint32_t, MTP::GenericObject*> _objectMap;
  map<uint32_t, MTP::Track*> _trackMap;
  void processErrorStack();

  vector <string> _errorStack;
  vector <string> _supportedFileTypes;
  vector <MTP::GenericObject*> _crossLinked;
  vector <MTP::Folder*> _rootFolders;
  vector <MTP::File*> _rootFiles;

  vector <MTP::File*> _files;
  vector <MTP::Track*> _tracks;
  vector <MTP::Album*> _albums;
  vector <MTP::Playlist*> _playlists;

  //Directory structure functions
  void createObjectStructure();
  void createFolderStructure(MTP::Folder*);
  void createFileStructure();
  void createAlbumStructure();
  void createPlaylistStructure();


  //Debug functions
  void dbgPrintSupportedFileTypes();
  void dbgPrintFolders(MTP::Folder*, count_t);
};
#endif 
