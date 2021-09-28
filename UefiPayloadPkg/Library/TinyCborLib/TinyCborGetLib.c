

#include "tinycbor\src\cbor.h"
#include <PiPei.h>

#include <Base.h>
#include <Library/DebugLib.h>
#include "Proto.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <UniversalPayload/SerialPortInfo.h>
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

  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *Serial;
  Serial = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));

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
  Serial->BaudRate       = (UINT32)result;

  cbor_value_map_find_value (&subMap, "RegisterBase", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "RegisterBase: 0x%lx \n", result ));
  Serial->RegisterBase   = (UINT64)result;

  cbor_value_map_find_value (&subMap, "RegisterStride", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "RegisterStride: 0x%lx \n", result ));
  Serial->RegisterStride = (UINT32)result;

  cbor_value_map_find_value (&subMap, "UseMmio", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "UseMmio: 0x%lx \n", result ));
  Serial->UseMmio        = (BOOLEAN)result;


  

  Serial->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
  Serial->Header.Length = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);



  
  return 0;
}
