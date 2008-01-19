#ifndef __MTPSUBSYSTEM__
#define __MTPSUBSYSTEM__

#include <libmtp.h>
#include <vector>
#include "types.h"
#include "MtpDevice.h"
#include <iostream>
using namespace std;

/** 
 * @class MtpSubSystem is a wrapper class around libmtp
*/
class MtpSubSystem
{
public:
  MtpSubSystem();
  ~MtpSubSystem();
  void ReleaseDevices();
  void Initialize();

  count_t DeviceCount() const;
  MtpDevice* GetDevice(count_t);

  LIBMTP_mtpdevice_t* _deviceList;
  MTPErrorVector _errorList;
private:
  vector <MtpDevice*> _devList;
};
#endif
