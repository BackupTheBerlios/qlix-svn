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
  /* not used 
  vector <wstring> _supportedFileTypes;
  vector <MTP::folder> _folders;
  vector <MTP::files> _files;
  vector <MTP::tracks> _tracks;
  vector <MTP::album> _albums;
  vector <MTP::playlist> _playlists;
  */
};
#endif 
