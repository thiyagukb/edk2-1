

#include "tinycbor\src\cbor.h"
#include <PiPei.h>

#include <Base.h>
#include <Library/DebugLib.h>
#include "Proto.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Include/CborParser.h>

CborParser parser;
CborValue value,subMap;

VOID *
Cbor_Parser_Init (
  OUT VOID                *Buffer,
  OUT UINTN               Size
  )
{
  cbor_parser_init(Buffer, Size, 0, &parser, &value);
  return &value;
}

VOID
Cbor_Get_Byte_by_String (
  IN VOID* Cborval,
  IN char * InputString,
  IN UINTN size,
  OUT UINT8* Buffer
)
{
  CborValue element;
  cbor_value_map_find_value ((CborValue*)Cborval, InputString, &element);
  cbor_value_copy_byte_string(&element, Buffer, &size, NULL);
}

VOID *
Cbor_Get_Submap_by_String (
  IN VOID* Rootmap,
  IN char * InputString
)
{
  cbor_value_map_find_value ((CborValue*)Rootmap, InputString, &subMap);
  return &subMap;
}

UINT64
Cbor_Get_Uint64_by_String (
  IN VOID* Cborval,
  IN char * InputString
)
{
  CborValue element;
  UINT64 result;
  cbor_value_map_find_value (&subMap, InputString, &element);
  cbor_value_get_uint64(&element, &result);
  return result;
}

INT64
Cbor_Get_int64_by_String (
  IN VOID* Cborval,
  IN char * InputString
)
{
  CborValue element;
  INT64 result;
  cbor_value_map_find_value (&subMap, InputString, &element);
  cbor_value_get_int64(&element, &result);
  return result;
}

int
Cbor_Get_int_by_String (
  IN VOID* Cborval,
  IN char * InputString
)
{
  CborValue element;
  int result;
  cbor_value_map_find_value (&subMap, InputString, &element);
  cbor_value_get_int(&element, &result);
  return result;
}