#ifndef REQUESTACTIONS_H
#define REQUESTACTIONS_H

#include <stdint.h>

typedef enum : uint8_t
{
    UNDEFINED = 0,
    PLAYLIST_ACTIONS,
    TRANSPORT_ACTIONS,
    FILE_ACTIONS,
    FIRMWARE_ACTIONS
}FrameType;

namespace Requests
{
enum class Transport
{
    PAUSE_PRINTING,
    REQUEST_PROGRESS
};

enum class Playlist
{
    REQUEST_PLAYLIST,
    REQUEST_PLAYLIST_POSITION,
    CHANGE_PLAYLIST,
    CHANGE_PLAYLIST_POSITION,
    CHANGE_PRINTNG_FILE
};

enum class File
{
    GET_FILE,
    GET_FOLDER_CONTENT,
    FILE_CREATE,
    FILE_APPEND_DATA
};

enum class Firmware
{
    FIRMWARE_VERSION,
    FIRMWARE_UPLOAD_START,
    FIRMWARE_UPLOAD_PROCEED,
    FIRMWARE_UPDATE
};

}

namespace Data
{
enum class Transport
{
    PROGRESS,
    PAUSE_STATE
};

enum class Playlist
{
    PLAYLIST,
    PLAYLIST_POSITION
};

enum class File
{
    REQUESTED_FILE,
    REQUESTED_FILE_NOT_FOUND,
    REQUESTED_FOLDER,
    REQUESTED_FOLDER_NOT_FOUND,
    UPLOAD_DATA_SAVED,
    UPLOAD_DATA_NOT_SAVED
};

enum class Firmware
{
    FIRMWARE_VERSION
};

}

#endif // REQUESTACTIONS_H
