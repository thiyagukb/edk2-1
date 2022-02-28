

#include "tinycbor\src\cbor.h"
#include <PiPei.h>
#include <Library/IoLib.h>
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
  size_t length;
  CborValue next;
  CborValue  	Array;
  IoWrite8(0x3f8,'Z');
  cbor_parser_init(Buffer, Size, 0, &parser, &value);
  IoWrite8(0x3f8,'X');
  CborValue  element,subMap;

  uint8_t   buf3[7];
  size_t   size3;
  size3 = 7;

UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *Serial;
  Serial = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));

  cbor_value_map_find_value (&value, "SerialPortBaudRate", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "SerialPortBaudRate: 0x%lx \n", result ));
  Serial->BaudRate       = (UINT32)result;
IoWrite8(0x3f8,'X');
  cbor_value_map_find_value (&value, "SerialPortRegisterBase", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "SerialPortRegisterBase: 0x%lx \n", result ));
  Serial->RegisterBase   = (UINT64)result;
IoWrite8(0x3f8,'X');
  cbor_value_map_find_value (&value, "SerialPortRegisterStride", &element);
  cbor_value_get_uint64(&element, &result);
  DEBUG ((EFI_D_ERROR, "SerialPortRegisterStride: 0x%lx \n", result ));
  Serial->RegisterStride = (UINT32)result;
IoWrite8(0x3f8,'X');
  cbor_value_map_find_value (&value, "SerialPortUseMmio", &element);
  cbor_value_get_boolean(&element, (bool *)&result);
  DEBUG ((EFI_D_ERROR, "SerialPortUseMmio: 0x%lx \n", result ));
  Serial->UseMmio        = (BOOLEAN)result;
IoWrite8(0x3f8,'X');
  
  
  cbor_value_map_find_value (&value, "UplExtradata", &element);
  cbor_value_enter_container	(	&element, &Array);

  cbor_value_map_find_value (&Array, "Identifier", &subMap);

  cbor_value_calculate_string_length(&subMap, &length);
  DEBUG ((EFI_D_ERROR, "lengtha: 0x%lx  subMap = 0x%lx  type: %x, remaining: %x\n", length , (UINT64) &subMap, subMap.type, subMap.remaining));
  cbor_value_copy_text_string	(	&subMap, (char *) (UINT8 *) buf3, &length,  &next);
  DEBUG ((EFI_D_ERROR, "Identifier string: %a\n", buf3));
  
  cbor_value_map_find_value (&Array, "Base", &subMap);
  cbor_value_get_uint64(&subMap, &result);
  DEBUG ((EFI_D_ERROR, "Base: 0x%lx \n", result ));

  cbor_value_map_find_value (&Array, "Size", &subMap);
  cbor_value_get_uint64(&subMap, &result);
  DEBUG ((EFI_D_ERROR, "Size: 0x%lx \n", result ));

  Serial->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
  Serial->Header.Length = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);


  return 0;
}
