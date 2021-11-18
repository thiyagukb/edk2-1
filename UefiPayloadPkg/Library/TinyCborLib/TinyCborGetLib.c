

#include "tinycbor\src\cbor.h"
#include <PiPei.h>

#include <Base.h>
#include <Library/DebugLib.h>
#include "Proto.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <UniversalPayload/SerialPortInfo.h>
#include <UniversalPayload/PciRootBridges.h>
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

  UINT8   buf3[30],buf4[sizeof(UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE)];
  UINTN   size3;
  size3 = 30;

  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *Serial;
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGES  *PciRootBridgeInfo;
  Serial = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));


  cbor_value_map_find_value (&value, "SerialPortInfoGuid", &element);
  size3 = 30;
  cbor_value_copy_byte_string(&element,  buf3, &size3, NULL);
  DEBUG ((EFI_D_ERROR, "SerialPortInfoGuid: 0x%g \n", buf3 ));

  cbor_value_map_find_value (&value, "SerialPortInfoGuidSubMap", &subMap);

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

  PciRootBridgeInfo = BuildGuidHob (&gUniversalPayloadPciRootBridgeInfoGuid, sizeof (PciRootBridgeInfo) + sizeof (UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE));
  
  cbor_value_map_find_value (&value, "PciRootBridgeInfoGuid", &element);
  cbor_value_copy_byte_string(&element,  buf3, &size3, NULL);
  DEBUG ((EFI_D_ERROR, "PciRootBridgeInfoGuid: 0x%g \n", buf3 ));


  size3 = sizeof(UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE);
  cbor_value_map_find_value (&value, "RootBridgeInfo", &element);
  cbor_value_copy_byte_string (&element,  (UINT8 *)buf4, &size3, NULL);
  CopyMem(PciRootBridgeInfo->RootBridge, buf4, size3);
  PrintHex1((UINT8 *)buf4,size3);

  cbor_value_map_find_value (&value, "PciRootBridgeInfoGuidSubMap", &subMap);

  cbor_value_map_find_value (&subMap, "RootBridgeCount", &element);
  cbor_value_get_uint64(&element, &result);
  PciRootBridgeInfo->Count = (UINT8)result;
  DEBUG ((EFI_D_ERROR, "RootBridgeCount: 0x%x \n", result ));

  cbor_value_map_find_value (&subMap, "HeaderLength", &element);
  cbor_value_get_uint64(&element, &result);
  PciRootBridgeInfo->Header.Length = result;
  DEBUG ((EFI_D_ERROR, "HeaderLength: 0x%x \n", result ));

  cbor_value_map_find_value (&subMap, "ResourceAssigned", &element);
  cbor_value_get_uint64(&element, &result);
  PciRootBridgeInfo->ResourceAssigned = result;
  DEBUG ((EFI_D_ERROR, "ResourceAssigned: 0x%x \n", result ));

  cbor_value_map_find_value (&subMap, "HeaderRevision", &element);
  cbor_value_get_uint64(&element, &result);
  PciRootBridgeInfo->Header.Revision =  result;
  DEBUG ((EFI_D_ERROR, "HeaderRevision: 0x%x \n", result ));
  


  DEBUG ((DEBUG_ERROR, "%a: PciRootBridgeInfo->Count: 0x%04x\n",  __FUNCTION__, PciRootBridgeInfo->Count));
  DEBUG ((DEBUG_ERROR, "%a: PciRootBridgeInfo->RootBridge[0].ResourceAssigned: 0x%04x\n",  __FUNCTION__, PciRootBridgeInfo->ResourceAssigned));
  DEBUG ((DEBUG_ERROR, "%a: PciRootBridgeInfo->RootBridge[0].ResourceAssigned: 0x%x\n",  __FUNCTION__, (UINTN)PciRootBridgeInfo->RootBridge[0].Bus.Limit));

  return 0;
}
