#include "MtpStorage.h"
MtpStorage::MtpStorage(LIBMTP_devicestorage_t* in_storage)
{
  strcpy(in_storage->StorageDescription, _description);
  strcpy(in_storage->VolumeIdentifier, _volumeID);
}

MtpStorage::~MtpStorage()
{
  delete _description;
  _description = NULL;
  delete _volumeID;
  _volumeID= NULL;
}



uint64_t MtpStorage::TotalSpace() const { return _totalSpace; }
uint64_t MtpStorage::FreeSpace() const { return _freeSpace; }
uint64_t MtpStorage::FreeObjectSpace() const { return _freeObjectSpace; }
unsigned int MtpStorage::ID() const {  return _id; }

const char* const MtpStorage::Description() const { return _description; }
const char* const MtpStorage::VolumeID() const  { return _volumeID;  }

