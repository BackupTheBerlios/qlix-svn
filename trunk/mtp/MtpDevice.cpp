#include "MtpDevice.h"
MtpDevice::MtpDevice( LIBMTP_mtpdevice_t* in_device, count_t in_id)
{
    _deviceID = in_id;
    if (!in_device)
        return;
    _device = in_device;
    getModelName();
    getSerialNum();
    getDeviceVersion();
    getSyncPartner();
    getFriendlyName();
    getBatteryLevel();
    GetFolders();
    LIBMTP_Get_Storage(in_device, 0);
}

MtpDevice::~MtpDevice()
{
    //remember to clear folders
    if (_device)
    {
        cout << "Releasing device" << endl;
        LIBMTP_Release_Device(_device);
        _device = NULL;
    }
}

count_t MtpDevice::GetID()
{
    return _deviceID;
}

void MtpDevice::DumpInformation ( void )
{ //i don't know why i formated this junk ;P
   cout << "Device serial:"         << _serial    << endl << "Model name:" << _modelName << endl << cout << "Device version:" << _deviceVersion << endl << "Device friendly name:"  << _friendlyName << endl << "Sync partner:" << _syncPartner << endl << "Current battery level:" << (count_t) _curBatteryLevel << endl << "Max battery level:"     << (count_t) _maxBatteryLevel << endl;
}

void MtpDevice::GetFolders ( void )
{
    LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
    GetErrors(_device);
    LIBMTP_file_t* files = LIBMTP_Get_Filelisting_With_Callback(_device, NULL, NULL);
    GetErrors(_device);
    _mtpFS = new MtpFS(folders, _device);
    _mtpFS->SetRootName(_friendlyName);
    _mtpFS->InsertFileList(files);
//        _mtpFS->Print();
}

MtpFS* MtpDevice::GetFileSystem ( void )
{
    return _mtpFS;
}

LIBMTP_mtpdevice_t* MtpDevice::rawDevice()
{
    return _device;
}

uint32_t MtpDevice::CreateFolder (string in_FolderName, uint32_t in_parentID, int* newModelIndex)
{
    if (!_device)
    {
        cout << "no device" << endl;
        return 0;
    }
    DirNode* parent = _mtpFS->GetDirectory(in_parentID);
    cout << "Creating folder- Parent is: " << parent->GetName() << endl;
    cout << "Creating folder- Parent ID: " << parent->GetID() << endl;

    char* foldername = const_cast<char*>(in_FolderName.c_str());
    LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
    int ret = LIBMTP_Create_Folder(_device, foldername, in_parentID);
    cout << "Created folder: " << ret << endl;
    for (count_t i=  0; i < 5 && (_mtpFS->GetDirectory(ret) != NULL); i ++)
    {
        cout << "Error device returned an ID thats already in use, retrying up to five times" << endl;
        LIBMTP_folder_t* folders = LIBMTP_Get_Folder_List(_device);
        ret = LIBMTP_Create_Folder(_device, foldername, in_parentID);
        cout << "Created folder: " << ret << endl;
        usleep(400);
    }

    if (_mtpFS->GetDirectory(ret) != NULL)
    {
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return 0;
    }

    if (ret == 0)
    {
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return ret;
    }
    (*newModelIndex) = _mtpFS->AddFolder(ret, in_parentID);

    return ret;
}

bool MtpDevice::DeleteObject (uint32_t in_parentID, int in_FileID)
{
    if (!_device)
    {
        cout << "no device" << endl;
        return false;
    }

    DirNode* parentDir = _mtpFS->GetDirectory(in_parentID);
    if (in_FileID == -1)
    {
        cout << "About to delete folder: " << parentDir->GetName() <<" with ID: " << parentDir->GetID() << endl;
        int ret = LIBMTP_Delete_Object(_device, in_parentID);
        if (ret != 0)
        {
            LIBMTP_Dump_Errorstack(_device);
            LIBMTP_Clear_Errorstack(_device);
            return false;
        }
        return true;
    }
    
    DirNode* current_dir =  _mtpFS->GetDirectory(in_parentID);
    if (current_dir == NULL)
    {
        cout << "Tried to delete child of a NULL folder! out of sync! " << endl;
        return false;
    }

    cout << "About to delete file with id: " << in_FileID<< endl;

    int ret = LIBMTP_Delete_Object(_device, in_FileID);
   // int ret = 0;
    if (ret != 0)
    {
       LIBMTP_Dump_Errorstack(_device);
       LIBMTP_Clear_Errorstack(_device);
       return false;
    }
    return true;
}


bool MtpDevice::SendFileToDevice(const QFileInfo& fileinfo,
                                 uint32_t in_parentID)
{
    if (!_device)
    {
        cout << "No device for file transfer" << endl;
        return -1;
    }

    //const char* filepath = (file.filePath().toStdString()).c_str();
    LIBMTP_file_t* mtpfile  = LIBMTP_new_file_t();

    string fn = fileinfo.fileName().toStdString();
    char* fnamebuf = new char[fn.length()+1];


    for (count_t i =0; i < fn.length(); i++)
    {
        fnamebuf[i] = fn[i];
    }
    fnamebuf[fn.length()] = '\0';

    cout << "filename is: " << fnamebuf << endl;

    mtpfile->filename = fnamebuf;
    mtpfile->parent_id = in_parentID;

    mtpfile->filesize = fileinfo.size();
    mtpfile->filetype = FileNode::GetMtpType((fileinfo.suffix()));
//    cout << "While sending file, type was set to: " << FileNode::GetMtpType(mtpfile->filetype) << endl;
    string wholefilepath = fileinfo.filePath().toStdString();
//    cout << "filepathis: " << wholefilepath<< endl;

    mtpfile->next = NULL;
    LIBMTP_progressfunc_t staticProgressFunc = MtpFS::ProgressWrapper; 

    int ret =LIBMTP_Send_File_From_File( _device, wholefilepath.c_str(), mtpfile,
                                         staticProgressFunc, (void*)_mtpFS, mtpfile->parent_id);
    cout <<"File transfer returned: " << ret << endl;
    if (ret != 0)
    {
        cout << "FAIL" << ret << endl;
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return false;
    }
    DirNode* parent = _mtpFS->GetDirectory(in_parentID);
    FileNode newFile(mtpfile, parent->GetFileCount());
    _mtpFS->AddFile(newFile);
    LIBMTP_destroy_file_t(mtpfile);
    return true;
}

bool MtpDevice::GetFileFromDevice (uint32_t in_ParentID, const string& target)
{
    if (!_device)
    {
        cout << "No device for file transfer" << endl;
        return -1;
    }

  //  funcPtr update = _mtpFS->getFunc();
    int ret = LIBMTP_Get_File_To_File( _device, in_ParentID,
                                       target.c_str(), NULL,NULL);

    if (ret != 0)
    {
        LIBMTP_Dump_Errorstack(_device);
        LIBMTP_Clear_Errorstack(_device);
        return false;
    }
    return true;
}


//private:
void MtpDevice::getModelName()
{
   char* temp;
   temp = LIBMTP_Get_Modelname (_device);
   _modelName = temp;
   if (temp)
       delete temp;
    else
       GetErrors(_device); 
}

void MtpDevice::getSerialNum()
{
    char* temp;
    temp = LIBMTP_Get_Serialnumber (_device);
    _serial = temp;
   if (temp)
       delete temp;
    else
       GetErrors(_device); 
    
} 

void MtpDevice::getDeviceVersion() 
{
    char* temp;
    temp = LIBMTP_Get_Deviceversion (_device);
    _deviceVersion = temp;
    if (temp)
       delete temp;
    else
       GetErrors(_device); 

}
void MtpDevice::getFriendlyName()
{
    char* temp;
    temp = LIBMTP_Get_Friendlyname (_device);
    _friendlyName = temp;
    if (temp)
       delete temp;
    else
       GetErrors(_device); 
}

void MtpDevice::getSyncPartner()
{
    return;
}

void MtpDevice::getBatteryLevel()
{
    int ret;
    ret = LIBMTP_Get_Batterylevel (_device, &_curBatteryLevel,
                                       &_maxBatteryLevel );
    if (ret != 0)
    {
       GetErrors(_device); 
    }
}
void MtpDevice::GetErrors (LIBMTP_mtpdevice_t* in_device)
{
    if (!in_device)
        return;
    LIBMTP_error_t* errors = LIBMTP_Get_Errorstack(in_device);
    while (errors)
    {
        string errstring = errors->error_text;
        cout <<"Qlix bridge error: " << errstring << endl;
        if (isTerminal(errors->errornumber) )
            exit(1);
        errors = errors->next;
    }
    LIBMTP_Clear_Errorstack(in_device);
}

bool MtpDevice::isTerminal (LIBMTP_error_number_t in_err)
{
    switch (in_err)
    {
        case LIBMTP_ERROR_USB_LAYER:
            return true;

        case LIBMTP_ERROR_MEMORY_ALLOCATION:
            return true;

        case LIBMTP_ERROR_NO_DEVICE_ATTACHED:
            return true;

        case LIBMTP_ERROR_CONNECTING:
            return true;
        default:
            return false;
    }
}

