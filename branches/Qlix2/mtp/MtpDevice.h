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

  char* name() const;
  char* serialNumber() const;
  char* version() const;
  char* syncPartner() const;
  char* modelName() const;

  bool BatterLevelSupport() const;
  float BatterLevel() const;

  void CreateObjectStructure();

  void SetProgressFunction(LIBMTP_progressfunc_t );

  void ClearObjectMappings();

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
  void createFolderStructure(MTP::Folder*);
  void createFileStructure();
  void createTrackStructure();



  //Debug functions
  void dbgPrintSupportedFileTypes();
  void dbgPrintFolders(MTP::Folder*, count_t);
};
#endif 
