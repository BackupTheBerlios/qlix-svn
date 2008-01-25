//TODO error checkking when returning char* ?
//TODO use multimap for track/file distinction
#include "mtp/MtpObject.h"
using namespace MTP;
namespace MTP
{

/** Creates a new GenericObject, a generic base class for MTP objects
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericObject::GenericObject(MtpObjectType in_type, uint32_t id) :
                             _type(in_type),
                             _id(id)
{ }

/** 
 * Returns the ID of this object
 */
count_t GenericObject::ID() { return _id; }

/** Simple function to get the type of the current MTP object
 * @return returns the type of the GenericObject
 */
MtpObjectType GenericObject::Type() { return _type; }


/** Creates a new Track object
 * @param in_track A pointer to the LIBMTP_track_t to wrap over
 * @return a new Track object
 */

Track::Track(LIBMTP_track_t* in_track) :            GenericObject(MtpTrack, in_track->item_id),
            _parentAlbum (NULL),
            _parentPlaylist(NULL)
{
  assert(in_track);
  _rawTrack = in_track;
}

/** Retreives the name of the wrapped Track
 * @return the tracks's UTF8 name 
 */
char const * const Track::Name() const
{
  return _rawTrack->title;
}

/** Returns the parent id of this track
 * @return the parent id of this track
 */
count_t Track::ParentFolderID() const { return _rawTrack->parent_id; }


/** Sets the parent album of this track
 * @param in_album the parent album of this track
 */
void Track::SetParentAlbum(Album* in_album) {_parentAlbum = in_album; }


/** Sets the parent playlist of this track
 * @param in_pl the parent playlist of this track
 */
void Track::SetParentPlaylist(Playlist* in_pl) {_parentPlaylist = in_pl; }




/** Returns the parent Album of this track
 * @return the parent Album of this track
 */
Album* Track::ParentAlbum() const { return _parentAlbum; }

/** Returns the parent Playlist of this track
 * @return the parent Playlist of this track
 */
Playlist* Track::ParentPlaylist() const { return _parentPlaylist; }


/** Creates a new File object and and stores its representative data
 * @param in_file A pointer to the LIBMTP_file_to wrap over
 * @param in_sample A pointer to the LIBMTP_filesampledata_t
 * @return a new File object
 */
File::File(LIBMTP_file_t* in_file, const LIBMTP_filesampledata_t& in_sample) : 
           GenericObject (MtpFile, in_file->item_id)
{
  _rawFile = in_file;
  _sampleData = in_sample;
  _parent = NULL;
}


/**
 * Retreives the file's parent ID
 * @return the file's parent ID it is zero if its in the root folder 
 */
count_t File::ParentID() const { return _rawFile->parent_id; }

/**
 * Retreives the file's parent Folder
 * @return the file's parent Folder or NULL if it exists in the root dir
 */
Folder* File::Parent() const { return _parent; }

/**
 * Sets the file's parent Folder
 */
void File::SetParent(Folder* in_parent) 
{ 
  _parent = in_parent;
}

/**
 * Returns the name of the file
 * @return The name of the file as a Utf8 string
 */
char const * const File::Name() const
{
  //TODO error checking here?
  return _rawFile->filename;
}

/** Creates a new Folder object
 * @param in_folder A pointer to the LIBMTP_folder_t wrap over
 * @param in_parent A pointer to this folder's parent
 * @return a new Folder object
 */
Folder::Folder(LIBMTP_folder_t* in_folder, Folder* in_parent) :
               GenericObject (MtpFolder, in_folder->folder_id)
{
  assert(in_folder);
  _rawFolder = in_folder;
  _parent = in_parent;
}
/**
 * Return's this folder's parent
 * @return the folder's parent
 */
Folder* Folder::Parent() const
{
  return _parent;
}

/**
 * Get the raw LIBMTP folder
 * @return the raw LIBMTP folder for this Folder object
 */
LIBMTP_folder_t* Folder::RawFolder() const
{
  return _rawFolder;
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
Folder* Folder::SubFolder(count_t idx) const
{
  if (idx >= _childFolders.size())
    return NULL;
  else
    return _childFolders[idx];
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
File* Folder::SubFile(count_t idx) const
{
  if (idx >= _childFiles.size())
    return NULL;
  else
    return _childFiles[idx];
}


/**
 * Returns the number of files under this folder
 * @return the number of files under this folder
 */
count_t Folder::FileCount() const
{
  return _childFiles.size();
}

/**
 * @return the number of folders under this folder
 */
count_t Folder::FolderCount() const
{
  return _childFolders.size();
}

/** Retreives the name of the wrapped folder or "" if it doesn't exist
 * @return the folder's UTF8 name or a blank string if it doesn't exist
 */
char const* const Folder::Name() const
{
  if (!_rawFolder)
    return "";
  else
    return _rawFolder->name;
}

/** Adds the passed folder as a subdirectory to this folder
 * @param in_folder the folder to add as a subfolder of this folder
 */
void Folder::AddChildFolder(Folder* in_folder)
{
  _childFolders.push_back(in_folder);
}

/** Adds the passed file as a leaf file of this
 * @param in_file the file to add as a leaf of this folder
 */
void Folder::AddChildFile(File* in_file)
{
  _childFiles.push_back(in_file);
}

/** Creates a new Album object
 * @param in_album A pointer to the LIBMTP_album_t wrap over
 * @return a new Album object
 */
Album::Album(LIBMTP_album_t* in_album, 
             const LIBMTP_filesampledata_t & in_sample) : 
             GenericObject (MtpAlbum, in_album->album_id),
             _sample(in_sample)
{
  assert(in_album);
  _rawAlbum = in_album;
  _initialized = false;
}


/** Returns the sample data for the album
 * @return a reference to the LIBMTP_sampledata_t that was pulled from 
 * the device
 */
const LIBMTP_filesampledata_t& Album::SampleData() const
{
  return _sample;
}

/** Adds the passed track as s subtrack to this album
 * @param in_track the track to ass as a subtrack to this folder
 */
void Album::AddChildTrack(Track* in_track) 
{
  _childTracks.push_back(in_track);
  in_track->SetParentAlbum(this);
  in_track->rowid = _childTracks.size();
}

/** Retreives the name of the wrapped Album
 * @return the album's UTF8 name 
 */
char const * const Album::Name() const
{
  return _rawAlbum->name;
}

/** Retreives the artist name of the wrapped Album
 * @return the albums's artist name in UTF8
 */
char const * const Album::Artist() const
{
  cout << "Artist: " << _rawAlbum->artist << endl;
  return _rawAlbum->artist;
}

/**
 * Albums are container objects that hold a list of tracks that 
 * reside underneath them
 * @return the track count under this album
 */ 
count_t Album::TrackCount() const 
{
  if (!_initialized)
   return _rawAlbum->no_tracks; 
  else
    return _childTracks.size();
}

/**
 * Albums are container objects that hold a list of track IDs 
 * @param idx the index of the requested track
 * @return the uint32_t track ID specified at the given index
 */ 
uint32_t Album::ChildTrackID(count_t idx) const
{
  assert(idx < TrackCount());
  return _rawAlbum->tracks[idx];
}


/**
 * Albums are also container objects that hold a list of tracks that 
 * reside underneath them
 * @param idx the index of the requested track
 * @return the Track* specified at the given index
 */ 
Track* Album::ChildTrack(count_t idx) const
{
  assert(idx < _childTracks.size());
  return _childTracks[idx];
}

/** 
 * The Initialized state tells us when to stop using the underlying 
 * LIBMTP data structure as it might become stale.
 */
void Album::SetInitialized() { _initialized = true; }

/** Creates a new Playlist object
 * @param in_pl t pointer to the LIBMTP_playlist_t wrap over
 * @return a new Playlist object
 */
Playlist::Playlist(LIBMTP_playlist_t* in_pl) : 
                  GenericObject(MtpPlaylist, in_pl->playlist_id)
{
  _initialized = false;
  _rawPlaylist =  in_pl;
}

/** Returns the name of this Playlist
 * @return the name of this playlist;
 */
char const * const Playlist::Name() const
{
  return _rawPlaylist->name;
}


/** Adds a track to the list of child tracks
 * @param in_track the pointer to the child track to add
 */
void Playlist::AddChildTrack(Track* in_track) 
{
  _childTracks.push_back(in_track);
  in_track->SetParentPlaylist(this);
}

/** Returns the child track at the given index
 * @param idx the index of the child track in the Playlists vector
 * @return the child tradck at the given index or null if it doesn't exist
 */
Track* Playlist::ChildTrack(count_t idx) const
{
  if (idx >= _childTracks.size())
    return NULL;
  return _childTracks[idx];
}
/**
 * Playlists are also container objects that hold a list of tracks that 
 * reside underneath them
 * @return the number of tracks underneath this playlist 
 */ 
count_t Playlist::TrackCount() const 
{
  if (!_initialized)
   return _rawPlaylist->no_tracks; 
  else
    return _childTracks.size();
}

/**
 * Playlists are container objects that hold a list of track IDs 
 * @param idx the index of the requested track id
 * @return the uint32_t track ID specified at the given index
 */ 
//there is a serious bug here if the trackcount is off from the underlying obj
uint32_t Playlist::ChildTrackID(count_t idx) const
{
  assert(idx < TrackCount());
  return _rawPlaylist->tracks[idx];
}

/** 
 * The Initialized state tells us when to stop using the underlying 
 * LIBMTP data structure as it might become stale.
 */
void Playlist::SetInitialized() { _initialized = true; }
}


