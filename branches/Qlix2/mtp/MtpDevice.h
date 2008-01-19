#ifndef __MTPDEVICE__
#define __MTPDEVICE__
#include <libmtp.h>
#include "types.h"
#include <string>
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
  vector <wstring> _supportedFileTypes;
  vector <wstring> _errorStack;
};
#endif 
