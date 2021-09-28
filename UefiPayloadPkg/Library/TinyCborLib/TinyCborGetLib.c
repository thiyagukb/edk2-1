

#include "tinycbor\src\cbor.h"


#include <Base.h>
#include <Library/DebugLib.h>
#include "Proto.h"
#include <Library/MemoryAllocationLib.h>
#define ROW_LIMITER 16




UINTN
PrintHex1 (
  IN  UINT8         *DataStart,
  IN  UINT16         DataSize
  )
{
  UINTN  Index1;
  UINTN  Index2;
  UINT8  *StartAddr;

  StartAddr = DataStart;
  for (Index1 = 0; Index1 * ROW_LIMITER < DataSize; Index1++) {
    DEBUG ((DEBUG_ERROR, "   0x%04p:", (DataStart - StartAddr)));
    for (Index2 = 0; (Index2 < ROW_LIMITER) && (Index1 * ROW_LIMITER + Index2 < DataSize); Index2++){
      DEBUG ((DEBUG_ERROR, " %02x", *DataStart));
      DataStart++;
    }
    DEBUG ((DEBUG_ERROR, "\n"));
  }

  return 0;
}



RETURN_STATUS
EFIAPI
GetCbor (
  OUT VOID                *Buffer,
  OUT UINTN               Size
  )
{

  

  CborParser parser;
  CborValue value;
  UINT64 result;
  cbor_parser_init(Buffer, Size, 0, &parser, &value);

  CborValue element, subMap;

  UINT8   buf3[30];
  UINTN   size3;
  size3 = 30;
  cbor_value_map_find_value (&value,  "RawByte", &element);
  cbor_value_copy_byte_string (&element,  buf3, &size3, NULL);
  PrintHex1(buf3,size3);

  cbor_value_map_find_value (&value, "MyGuid", &element);
  size3 = 30;
  cbor_value_copy_byte_string(&element,  buf3, &size3, NULL);
  DEBUG ((EFI_D_ERROR, "MyGuid: 0x%g \n", buf3 ));

  cbor_value_map_find_value (&value, "SubCbor", &subMap);

  cbor_value_map_find_value (&subMap, "BaudRate", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "BaudRate: 0x%lx \n", result ));

  cbor_value_map_find_value (&subMap, "RegisterBase", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "RegisterBase: 0x%lx \n", result ));

  cbor_value_map_find_value (&subMap, "RegisterStride", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "RegisterStride: 0x%lx \n", result ));

  cbor_value_map_find_value (&subMap, "UseMmio", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "UseMmio: 0x%lx \n", result ));






  
  return 0;
}
