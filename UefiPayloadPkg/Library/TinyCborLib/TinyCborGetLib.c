

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
  IoWrite8 (0x3F8, 65);
  cbor_parser_init(Buffer, Size, 0, &parser, &value);
  IoWrite8 (0x3F8, 66);
  CborValue element;//, subMap;

  //UINT8   buf3[30];
  UINTN   size3;
  size3 = 30;

  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *Serial;
  IoWrite8 (0x3F8, 67);
  Serial = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));
  IoWrite8 (0x3F8, 68);

 /* cbor_value_map_find_value (&value,  "RawByte", &element);
  cbor_value_copy_byte_string (&element,  buf3, &size3, NULL);
  PrintHex1(buf,size3);*/
  
  IoWrite8 (0x3F8, 69);
  cbor_value_map_find_value (&value, "SerialPortInfo.Mmio", &element);
  IoWrite8 (0x3F8, 70);
  cbor_value_get_boolean(&element,(BOOLEAN*)&result);
  IoWrite8 (0x3F8, 71);
  DEBUG ((EFI_D_ERROR, "SerialPortInfo.Mmio: 0x%d \n", result ));
  IoWrite8 (0x3F8, 72);
  Serial->UseMmio        = (BOOLEAN)result;
  
  IoWrite8 (0x3F8, 73);
  cbor_value_map_find_value (&value, "SerialPortInfo.RegisterStride", &element);
  IoWrite8 (0x3F8, 74);
  cbor_value_get_uint64(&element,&result);
  IoWrite8 (0x3F8, 75);
  DEBUG ((EFI_D_ERROR, "SerialPortInfo.RegisterStride: 0x%d \n", result ));
  Serial->RegisterStride = (UINT32)result;

  IoWrite8 (0x3F8, 76);
  cbor_value_map_find_value (&value, "SerialPortInfo.BaudRate", &element);
  IoWrite8 (0x3F8, 77);
  cbor_value_get_uint64(&element,&result);
  IoWrite8 (0x3F8, 78);
  DEBUG ((EFI_D_ERROR, "SerialPortInfo.BaudRate: 0x%d \n", result ));
  Serial->BaudRate       = (UINT32)result;

  IoWrite8 (0x3F8, 79);
  cbor_value_map_find_value (&value, "SerialPortInfo.RegisterBase", &element);
  IoWrite8 (0x3F8, 80);
  cbor_value_get_uint64(&element,&result);
  IoWrite8 (0x3F8, 81);
  DEBUG ((EFI_D_ERROR, "SerialPortInfo.RegisterBase: 0x%d \n", result ));
  Serial->RegisterBase   = (UINT64)result;
  //cbor_value_map_find_value (&value, "SubCbor", &subMap);

  /*cbor_value_map_find_value (&subMap, "BaudRate", &element);
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
  Serial->UseMmio        = (BOOLEAN)result;*/


  
  IoWrite8 (0x3F8, 82);
  Serial->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
  Serial->Header.Length = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);

IoWrite8 (0x3F8, 83);

  
  return 0;
}
