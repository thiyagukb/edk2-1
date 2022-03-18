#include <PiPei.h>

#include <Base.h>
#include <Library/DebugLib.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <UniversalPayload/SerialPortInfo.h>
#include <Library/GetUplDataLib.h>
#include "CborGetWrapper.h"
#include <Library/IoLib.h>

#define RETURN_ON_ERROR(Expression)          \
    do {                                 \
      Status = Expression;               \
      if (Status != RETURN_SUCCESS) {    \
        return Status;                   \
      }                                  \
    } while (FALSE)

UINT64
EFIAPI
InitUplFromBuffer (
  IN VOID   *Buffer
  )
{
  RETURN_STATUS Status;
  UINT64        BufferSize;
  Status = CborDecoderGetRootMap (Buffer, SIZE_1KB);
  if (RETURN_ERROR(Status)) {
    return 0;
  }
  Status = GetUplUint64("BufferSize", &BufferSize);
  if (RETURN_ERROR(Status)) {
    return 0;
  }
  Status = CborDecoderGetRootMap (Buffer, BufferSize);
  if (RETURN_ERROR(Status)) {
    return 0;
  }
  return BufferSize;
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
  IN  CHAR8  *String,
  OUT UINT8  *Result
  )
{
  return CborDecoderGetUint8 (String, Result, NULL);
}

RETURN_STATUS
EFIAPI
GetUplBoolean (
  IN  CHAR8    *String,
  OUT BOOLEAN  *Result
  )
{
  return CborDecoderGetBoolean (String, Result, NULL);
}

RETURN_STATUS
EFIAPI
GetUplBinary (
  IN     CHAR8  *String,
  IN OUT VOID   *Buffer,
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
  IN OUT UINTN                               *Count,
  IN     UINTN                               Index
  )
{
  UINTN             Size, LocalIndex;
  UPL_DATA_DECODER  SubMap;
  UINTN             IdentifierSize;
  RETURN_STATUS     Status;

  RETURN_ON_ERROR (CborDecoderGetArrayLengthAndFirstElement ("UplExtradata", &Size, &SubMap));
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
    RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
    }

    RETURN_ON_ERROR (CborDecoderGetUint64 ("Base", &Data[LocalIndex].Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Size", &Data[LocalIndex].Size, &SubMap));
    IdentifierSize = 16;
    RETURN_ON_ERROR (CborDecoderGetTextString ("Identifier", (UINT8 *)&Data[LocalIndex].Identifier, &IdentifierSize, &SubMap));
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
  RETURN_STATUS     Status;

  RETURN_ON_ERROR (CborDecoderGetArrayLengthAndFirstElement ("MemoryMap", &Size, &SubMap));
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
    RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
    }

    RETURN_ON_ERROR (CborDecoderGetUint64 ("Base", &Data[LocalIndex].PhysicalStart, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("NumberOfPages", &Data[LocalIndex].NumberOfPages, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Type", &Type, &SubMap));
    Data[LocalIndex].Type = (UINT32)Type;
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Attribute", &Data[LocalIndex].Attribute, &SubMap));
  }

  *Count = LocalIndex;

  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
GetUplPciRootBridges (
  IN OUT UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE  *Data,
  IN OUT UINTN                              *Count,
  IN     UINTN                              Index
  )
{
  UINTN             Size, LocalIndex;
  UPL_DATA_DECODER  SubMap;
  RETURN_STATUS     Status;

  RETURN_ON_ERROR (CborDecoderGetArrayLengthAndFirstElement ("RootBridgeInfo", &Size, &SubMap));
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
    RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
    }

    RETURN_ON_ERROR (CborDecoderGetUint64 ("Segment", (UINT64 *)&Data[LocalIndex].Segment, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Supports", &Data[LocalIndex].Supports, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Attributes", &Data[LocalIndex].Attributes, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetBoolean ("DmaAbove4G", &Data[LocalIndex].DmaAbove4G, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetBoolean ("NoExtendedConfigSpace", &Data[LocalIndex].NoExtendedConfigSpace, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("AllocationAttributes", &Data[LocalIndex].AllocationAttributes, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("BusBase", &Data[LocalIndex].Bus.Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("BusLimit", &Data[LocalIndex].Bus.Limit, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("BusTranslation", &Data[LocalIndex].Bus.Translation, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("IoBase", &Data[LocalIndex].Io.Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("IoLimit", &Data[LocalIndex].Io.Limit, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("IoTranslation", &Data[LocalIndex].Io.Translation, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("MemBase", &Data[LocalIndex].Mem.Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("MemLimit", &Data[LocalIndex].Mem.Limit, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("MemTranslation", &Data[LocalIndex].Mem.Translation, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("MemAbove4GBase", &Data[LocalIndex].MemAbove4G.Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("MemAbove4GLimit", &Data[LocalIndex].MemAbove4G.Limit, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("MemAbove4GTranslation", &Data[LocalIndex].MemAbove4G.Translation, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("PMemBase", &Data[LocalIndex].PMem.Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("PMemLimit", &Data[LocalIndex].PMem.Limit, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("PMemTranslation", &Data[LocalIndex].PMem.Translation, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("PMemAbove4GBase", &Data[LocalIndex].PMemAbove4G.Base, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("PMemAbove4GLimit", &Data[LocalIndex].PMemAbove4G.Limit, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("PMemAbove4GTranslation", &Data[LocalIndex].PMemAbove4G.Translation, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("HID", (UINT64 *)&Data[LocalIndex].HID, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("UID", (UINT64 *)&Data[LocalIndex].UID, &SubMap));
  }

  *Count = LocalIndex;

  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
GetUplResourceData (
  IN OUT UNIVERSAL_PAYLOAD_RESOURCE_DESCRIPTOR  *Data,
  IN OUT UINTN                                  *Count,
  IN     UINTN                                  Index
  )
{
  UINTN             Size, BinarySize, LocalIndex;
  UPL_DATA_DECODER  SubMap;
  RETURN_STATUS     Status;

  RETURN_ON_ERROR (CborDecoderGetArrayLengthAndFirstElement ("Resource", &Size, &SubMap));
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
    RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
    }

    BinarySize = sizeof (Data[LocalIndex].Owner);
    RETURN_ON_ERROR (CborDecoderGetBinary ("Owner", &Data[LocalIndex].Owner, &BinarySize, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Type", (UINT64 *)&Data[LocalIndex].ResourceType, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Attribute", (UINT64 *)&Data[LocalIndex].ResourceAttribute, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Base", (UINT64 *)&Data[LocalIndex].PhysicalStart, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Length", (UINT64 *)&Data[LocalIndex].ResourceLength, &SubMap));
  }

  *Count = LocalIndex;

  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
GetUplMemoryAllocationData (
  IN OUT UNIVERSAL_PAYLOAD_MEMORY_ALLOCATION  *Data,
  IN OUT UINTN                                *Count,
  IN     UINTN                                Index
  )
{
  UINTN             Size, BinarySize, LocalIndex;
  UPL_DATA_DECODER  SubMap;
  RETURN_STATUS     Status;

  RETURN_ON_ERROR (CborDecoderGetArrayLengthAndFirstElement ("ResourceAllocation", &Size, &SubMap));
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
    RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
  }

  for (LocalIndex = 0; LocalIndex < *Count && LocalIndex < (Size -Index); LocalIndex++) {
    if (LocalIndex != 0) {
      RETURN_ON_ERROR (CborDecoderGetArrayNextMap (&SubMap));
    }

    BinarySize = sizeof (Data[LocalIndex].Name);
    RETURN_ON_ERROR (CborDecoderGetBinary ("Name", &Data[LocalIndex].Name, &BinarySize, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Base", (UINT64 *)&Data[LocalIndex].MemoryBaseAddress, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Length", (UINT64 *)&Data[LocalIndex].MemoryLength, &SubMap));
    RETURN_ON_ERROR (CborDecoderGetUint64 ("Type", (UINT64 *)&Data[LocalIndex].MemoryType, &SubMap));
  }

  *Count = LocalIndex;

  return EFI_SUCCESS;
}
