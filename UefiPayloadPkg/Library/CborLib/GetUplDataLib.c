#include <PiPei.h>

#include <Base.h>
#include <Library/DebugLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <UniversalPayload/SerialPortInfo.h>
#include <Library/GetUplDataLib.h>
#include "CborGetWrapper.h"
#include <Library/IoLib.h>

#define ROW_LIMITER  16

RETURN_STATUS
EFIAPI
InitUplFromBuffer (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
{
  return CborDecoderGetRootMap (Buffer, Size);
}

RETURN_STATUS
EFIAPI
GetUplUint64 (
  IN  CHAR8   *String,
  OUT UINT64  *Result
  )
{
  return CborDecoderGetUint64 (String, Result, NULL);
}

RETURN_STATUS
EFIAPI
GetUplUint8 (
  IN  CHAR8   *String,
  OUT UINT8   *Result
  )
{
  return CborDecoderGetUint8 (String, Result, NULL);
}

RETURN_STATUS
EFIAPI
GetUplBoolean (
  IN  CHAR8     *String,
  OUT BOOLEAN   *Result
  )
{
  return CborDecoderGetBoolean (String, Result, NULL);
}

RETURN_STATUS
EFIAPI
GetUplBinary (
  IN     CHAR8  *String,
  IN OUT VOID  *Buffer,
  IN OUT UINTN  *Size
  )
{
  return CborDecoderGetBinary (String, Buffer, Size, NULL);
}

RETURN_STATUS
EFIAPI
GetUplAsciiString (
  IN     CHAR8  *String,
  IN OUT UINT8  *Buffer,
  IN OUT UINTN  *Size
  )
{
  return CborDecoderGetTextString (String, Buffer, Size, NULL);
}

RETURN_STATUS
EFIAPI
GetUplExtraData (
  IN OUT UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY  *Data,
  IN OUT UINTN                                    *Count,
  IN     UINTN                                    Index
  )
{
  UINTN             Size, LocalIndex;
  UPL_DATA_DECODER  SubMap;
  UINTN             IdentifierSize;

  CborDecoderGetArrayLengthAndFirstElement ("UplExtradata", &Size, &SubMap);
  if (*Count == 0) {
    *Count = Size;
    return RETURN_BUFFER_TOO_SMALL;
  }

  if (Index >= Size) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Skip the first Index element.
  //
  for (LocalIndex = 0; LocalIndex < Index; LocalIndex++) {
    CborDecoderGetArrayNextMap (&SubMap);
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      CborDecoderGetArrayNextMap (&SubMap);
    }

    CborDecoderGetUint64 ("Base", &Data[LocalIndex].Base, &SubMap);
    CborDecoderGetUint64 ("Size", &Data[LocalIndex].Size, &SubMap);
    IdentifierSize = 16;
    CborDecoderGetTextString ("Identifier", (UINT8 *)&Data[LocalIndex].Identifier, &IdentifierSize, &SubMap);
  }

  *Count = LocalIndex;

  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
GetUplMemoryMap (
  IN OUT EFI_MEMORY_DESCRIPTOR  *Data,
  IN OUT UINTN                  *Count,
  IN     UINTN                  Index
  )
{
  UINTN             Size, LocalIndex;
  UPL_DATA_DECODER  SubMap;
  UINT64            Type;

  CborDecoderGetArrayLengthAndFirstElement ("MemoryMap", &Size, &SubMap);
  if (*Count == 0) {
    *Count = Size;
    return RETURN_BUFFER_TOO_SMALL;
  }

  if (Index >= Size) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Skip the first Index element.
  //
  for (LocalIndex = 0; LocalIndex < Index; LocalIndex++) {
    CborDecoderGetArrayNextMap (&SubMap);
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      CborDecoderGetArrayNextMap (&SubMap);
    }

    CborDecoderGetUint64 ("Base", &Data[LocalIndex].PhysicalStart, &SubMap);
    CborDecoderGetUint64 ("NumberOfPages", &Data[LocalIndex].NumberOfPages, &SubMap);
    CborDecoderGetUint64 ("Type", &Type, &SubMap);
    Data[LocalIndex].Type = (UINT32) Type;
    CborDecoderGetUint64 ("Attribute", &Data[LocalIndex].Attribute, &SubMap);
  }

  *Count = LocalIndex;

  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
GetUplPciRootBridges (
  IN OUT UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE  *Data,
  IN OUT UINTN                                    *Count,
  IN     UINTN                                    Index
  )
{
  UINTN             Size, LocalIndex;
  UPL_DATA_DECODER  SubMap;

  CborDecoderGetArrayLengthAndFirstElement ("RootBridgeInfo", &Size, &SubMap);
  if (*Count == 0) {
    *Count = Size;
    return RETURN_BUFFER_TOO_SMALL;
  }

  if (Index >= Size) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Skip the first Index element.
  //
  for (LocalIndex = 0; LocalIndex < Index; LocalIndex++) {
    CborDecoderGetArrayNextMap (&SubMap);
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      CborDecoderGetArrayNextMap (&SubMap);
    }

    CborDecoderGetUint64 ("Segment", (UINT64 *)&Data[LocalIndex].Segment, &SubMap);
    CborDecoderGetUint64 ("Supports", &Data[LocalIndex].Supports, &SubMap);
    CborDecoderGetUint64 ("Attributes", &Data[LocalIndex].Attributes, &SubMap);
    CborDecoderGetBoolean ("DmaAbove4G", &Data[LocalIndex].DmaAbove4G, &SubMap);
    CborDecoderGetBoolean ("NoExtendedConfigSpace", &Data[LocalIndex].NoExtendedConfigSpace, &SubMap);
    CborDecoderGetUint64 ("AllocationAttributes", &Data[LocalIndex].AllocationAttributes, &SubMap);
    CborDecoderGetUint64 ("BusBase", &Data[LocalIndex].Bus.Base, &SubMap);
    CborDecoderGetUint64 ("BusLimit", &Data[LocalIndex].Bus.Limit, &SubMap);
    CborDecoderGetUint64 ("BusTranslation", &Data[LocalIndex].Bus.Translation, &SubMap);
    CborDecoderGetUint64 ("IoBase", &Data[LocalIndex].Io.Base, &SubMap);
    CborDecoderGetUint64 ("IoLimit", &Data[LocalIndex].Io.Limit, &SubMap);
    CborDecoderGetUint64 ("IoTranslation", &Data[LocalIndex].Io.Translation, &SubMap);
    CborDecoderGetUint64 ("MemBase", &Data[LocalIndex].Mem.Base, &SubMap);
    CborDecoderGetUint64 ("MemLimit", &Data[LocalIndex].Mem.Limit, &SubMap);
    CborDecoderGetUint64 ("MemTranslation", &Data[LocalIndex].Mem.Translation, &SubMap);
    CborDecoderGetUint64 ("MemAbove4GBase", &Data[LocalIndex].MemAbove4G.Base, &SubMap);
    CborDecoderGetUint64 ("MemAbove4GLimit", &Data[LocalIndex].MemAbove4G.Limit, &SubMap);
    CborDecoderGetUint64 ("MemAbove4GTranslation", &Data[LocalIndex].MemAbove4G.Translation, &SubMap);
    CborDecoderGetUint64 ("PMemBase", &Data[LocalIndex].PMem.Base, &SubMap);
    CborDecoderGetUint64 ("PMemLimit", &Data[LocalIndex].PMem.Limit, &SubMap);
    CborDecoderGetUint64 ("PMemTranslation", &Data[LocalIndex].PMem.Translation, &SubMap);
    CborDecoderGetUint64 ("PMemAbove4GBase", &Data[LocalIndex].PMemAbove4G.Base, &SubMap);
    CborDecoderGetUint64 ("PMemAbove4GLimit", &Data[LocalIndex].PMemAbove4G.Limit, &SubMap);
    CborDecoderGetUint64 ("PMemAbove4GTranslation", &Data[LocalIndex].PMemAbove4G.Translation, &SubMap);
    CborDecoderGetUint64 ("HID", (UINT64 *)&Data[LocalIndex].HID, &SubMap);
    CborDecoderGetUint64 ("UID", (UINT64 *)&Data[LocalIndex].UID, &SubMap);
  }
  *Count = LocalIndex;

  return EFI_SUCCESS;
}


RETURN_STATUS
EFIAPI
GetUplResourceData (
  IN OUT EFI_HOB_RESOURCE_DESCRIPTOR_DATA         *Data,
  IN OUT UINTN                                    *Count,
  IN     UINTN                                    Index
  )
{
  UINTN             Size,BinarySize, LocalIndex;
  UPL_DATA_DECODER  SubMap;

  CborDecoderGetArrayLengthAndFirstElement ("Resource", &Size, &SubMap);
  if (*Count == 0) {
    *Count = Size;
    return RETURN_BUFFER_TOO_SMALL;
  }

  if (Index >= Size) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Skip the first Index element.
  //
  for (LocalIndex = 0; LocalIndex < Index; LocalIndex++) {
    CborDecoderGetArrayNextMap (&SubMap);
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      CborDecoderGetArrayNextMap (&SubMap);
    }
    BinarySize = sizeof(Data[LocalIndex].Owner);
    CborDecoderGetBinary ("Owner", &Data[LocalIndex].Owner,&BinarySize, &SubMap);
    CborDecoderGetUint64 ("Type", (UINT64 *)&Data[LocalIndex].ResourceType, &SubMap);
    CborDecoderGetUint64 ("Attribute",(UINT64 *) &Data[LocalIndex].ResourceAttribute, &SubMap);
    CborDecoderGetUint64 ("Base", (UINT64 *)&Data[LocalIndex].PhysicalStart, &SubMap);
    CborDecoderGetUint64 ("Length", (UINT64 *)&Data[LocalIndex].ResourceLength, &SubMap);

  }
  *Count = LocalIndex;

  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
GetUplMemoryAllocationData (
  IN OUT EFI_HOB_MEMORY_ALLOCATION_DATA           *Data,
  IN OUT UINTN                                    *Count,
  IN     UINTN                                    Index
  )
{
  UINTN             Size,BinarySize, LocalIndex;
  UPL_DATA_DECODER  SubMap;

  CborDecoderGetArrayLengthAndFirstElement ("ResourceAllocation", &Size, &SubMap);
  if (*Count == 0) {
    *Count = Size;
    return RETURN_BUFFER_TOO_SMALL;
  }

  if (Index >= Size) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Skip the first Index element.
  //
  for (LocalIndex = 0; LocalIndex < Index; LocalIndex++) {
    CborDecoderGetArrayNextMap (&SubMap);
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      CborDecoderGetArrayNextMap (&SubMap);
    }
    BinarySize = sizeof(Data[LocalIndex].Name);
    CborDecoderGetBinary ("Name", &Data[LocalIndex].Name,&BinarySize, &SubMap);
    CborDecoderGetUint64 ("Base", (UINT64 *)&Data[LocalIndex].MemoryBaseAddress, &SubMap);
    CborDecoderGetUint64 ("Length", (UINT64 *)&Data[LocalIndex].MemoryLength, &SubMap);
    CborDecoderGetUint64 ("Type", (UINT64 *)&Data[LocalIndex].MemoryType, &SubMap);

  }
  *Count = LocalIndex;

  return EFI_SUCCESS;
}

VOID
EFIAPI
GetCbor (
  OUT VOID   *Buffer,
  OUT UINTN  Size
  )
{
 
}
