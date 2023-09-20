
#ifndef GJ_VFS_DEFS_HEADER
#define GJ_VFS_DEFS_HEADER

#include "GjDefs.h"

namespace yaglib
{
  namespace vfs
  {

#define PATH_SEPARATOR  L"/"

// Header
const char cDefSignature[] = "GJ!VFS!";
typedef struct 
{
  char signature[8];
  int headerSize;
  int folderOffset;
  int dataOffset;
} VFS_HEADER, *PVFS_HEADER;

  } /* namespace vfs */
} /* namespace yaglib */


#endif /* GJ_VFS_DEFS_HEADER */
