#include <PiPei.h>
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/CborRootmapHobGuid.h>
#include "CborSetWrapper.h"
#include <Library/SetUplDataLib.h>

#define RETURN_ON_ERROR(Expression)          \
    do {                                 \
      Status = Expression;               \
      if (Status != RETURN_SUCCESS) {    \
        return Status;                   \
      }                                  \
    } while (FALSE)

VOID  *gRootEncoderPointer, *RootMapEncoderPointer;
VOID  *gBuffer;

RETURN_STATUS
EFIAPI
SetUplDataLibConstructor (
  VOID
  )
{
  VOID               *Hob;
  UINTN              Size;
  CBOR_ROOTMAP_INFO  *Data;
  RETURN_STATUS      Status;

  Size = PcdGet32(PcdUniversalPayloadInterfaceSize);
  DEBUG ((DEBUG_INFO, "KBT In TinyCborEncoderLibConstructor.\n"));
  Hob = (VOID *)GetFirstGuidHob (&CborRootmapHobGuid);
  if (Hob == NULL) {
    RETURN_ON_ERROR (CborEncoderInit (&gRootEncoderPointer, &RootMapEncoderPointer, &gBuffer, Size));
    Data                 = (CBOR_ROOTMAP_INFO *)BuildGuidHob (&CborRootmapHobGuid, sizeof (CBOR_ROOTMAP_INFO));
    Data->RootEncoder    = gRootEncoderPointer;
    Data->RootMapEncoder = RootMapEncoderPointer;
    Data->Buffer         = gBuffer;
    SetUplUint64(BUFFER_SIZE, Size);
  } else {
    Data                  = GET_GUID_HOB_DATA (Hob);
    gRootEncoderPointer    = Data->RootEncoder;
    RootMapEncoderPointer = Data->RootMapEncoder;
    gBuffer                = Data->Buffer;
  }

  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplUint64 (
  IN CHAR8   *Key,
  IN UINT64  Value
  )
{
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, Key));
  RETURN_ON_ERROR (CborEncodeUint64 (RootMapEncoderPointer, Value));
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplUint8 (
  IN CHAR8  *Key,
  IN UINT8  Value
  )
{
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, Key));
  RETURN_ON_ERROR (CborEncodeUint8 (RootMapEncoderPointer, Value));
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplBinary (
  IN CHAR8  *Key,
  IN VOID   *Value,
  IN UINTN  Size
  )
{
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, Key));
  RETURN_ON_ERROR (CborEncodeByteString (RootMapEncoderPointer, Value, Size));
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplAsciiString (
  IN CHAR8  *Key,
  IN UINT8  *Value,
  IN UINTN  Size
  )
{
  // todo: Size need be used.
  // todo :if value don't have a terminal character, Size will be the maxium number
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, Key));
  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, (const char *)Value));
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplBoolean (
  IN CHAR8    *Key,
  IN BOOLEAN  Value
  )
{
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, Key));
  RETURN_ON_ERROR (CborEncodeBoolean (RootMapEncoderPointer, Value));
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
LockUplAndGetBuffer (
  OUT VOID   **Buff,
  OUT UINTN  *Size
  )
{
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncoderCloseContainer (gRootEncoderPointer, RootMapEncoderPointer));
  *Size  = CborGetBuffer (gRootEncoderPointer, gBuffer);
  *Buff  = gBuffer;
  return RETURN_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplExtraData (
  IN UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY  *Data,
  IN UINTN                               Count
  )
{
  VOID           *ArrayEncoder;
  VOID           *SubMapEncoder;
  UINTN          Index;
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, UPL_EXTRA_DATA));
  RETURN_ON_ERROR (CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count));
  for (Index = 0; Index < Count; Index++) {
    RETURN_ON_ERROR (CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, IDENTIFIER));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, Data[Index].Identifier));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, SIZE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Size));
    RETURN_ON_ERROR (CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder));
  }

  RETURN_ON_ERROR (CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder));
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplMemoryMap (
  IN EFI_MEMORY_DESCRIPTOR  *Data,
  IN UINTN                  Count
  )
{
  VOID           *ArrayEncoder;
  VOID           *SubMapEncoder;
  UINTN          Index;
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, MEMORY_MAP));
  RETURN_ON_ERROR (CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count));
  for (Index = 0; Index < Count; Index++) {
    RETURN_ON_ERROR (CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PhysicalStart));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, NUMBER_OF_PAGES));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].NumberOfPages));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, TYPE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Type));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, ATTRIBUTE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Attribute));
    RETURN_ON_ERROR (CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder));
  }

  RETURN_ON_ERROR (CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder));
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplPciRootBridges (
  IN UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE  *Data,
  IN UINTN                              Count
  )
{
  VOID           *ArrayEncoder;
  VOID           *SubMapEncoder;
  UINTN          Index;
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, ROOT_BRIDGE_INFO));
  RETURN_ON_ERROR (CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count));

  for (Index = 0; Index < Count; Index++) {
    RETURN_ON_ERROR (CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, SEGMENT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Segment));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, SUPPORTS));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Supports));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, ATTRIBUTES));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Attributes));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, DMA_ABOVE_4G));
    RETURN_ON_ERROR (CborEncodeBoolean (SubMapEncoder, Data[Index].DmaAbove4G));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, NO_EXTENDED_CONFIG_SPACE));
    RETURN_ON_ERROR (CborEncodeBoolean (SubMapEncoder, Data[Index].NoExtendedConfigSpace));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, ALLOCATION_ATTRIBUTES));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].AllocationAttributes));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BUS_BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Bus.Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BUS_LIMIT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Bus.Limit));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BUS_TRANSLATION));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Bus.Translation));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, IO_BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Io.Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, IO_LIMIT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Io.Limit));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, IO_TRANSLATION));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Io.Translation));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, MEM_BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Mem.Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, MEM_LIMIT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Mem.Limit));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, MEM_TRANSLATION));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].Mem.Translation));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, MEM_ABOVE_4G_BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].MemAbove4G.Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, MEM_ABOVE_4G_LIMIT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].MemAbove4G.Limit));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, MEM_ABOVE_4G_TRANSLATION));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].MemAbove4G.Translation));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, PMEM_BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PMem.Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, PMEM_LIMIT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PMem.Limit));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, PMEM_TRANSLATION));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PMem.Translation));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, PMEM_ABOVE_4G_BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PMemAbove4G.Base));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, PMEM_ABOVE_4G_LIMIT));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PMemAbove4G.Limit));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, PMEM_ABOVE_4G_TRANSLATION));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PMemAbove4G.Translation));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, ROOT_BRIDGE_HID));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].HID));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, ROOT_BRIDGE_UID));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].UID));
    RETURN_ON_ERROR (CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder));
  }

  RETURN_ON_ERROR (CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder));
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplResourceData (
  IN UNIVERSAL_PAYLOAD_RESOURCE_DESCRIPTOR  *Data,
  IN UINTN                                  Count
  )
{
  VOID           *ArrayEncoder;
  VOID           *SubMapEncoder;
  UINTN          Index;
  RETURN_STATUS  Status;

  DEBUG ((EFI_D_ERROR, "KBT Data->Owner: %g \n", &Data->Owner));
  DEBUG ((EFI_D_ERROR, "KBT sizeof(Data[Index].Owner: %x \n", sizeof (Data[0].Owner)));
  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, RESOURCE));
  RETURN_ON_ERROR (CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count));
  for (Index = 0; Index < Count; Index++) {
    RETURN_ON_ERROR (CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, OWNER));
    RETURN_ON_ERROR (CborEncodeByteString (SubMapEncoder, (UINT8 *)&Data[Index].Owner, sizeof (Data[Index].Owner)));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, TYPE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].ResourceType));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, ATTRIBUTE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].ResourceAttribute));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].PhysicalStart));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, LENGTH));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].ResourceLength));
    RETURN_ON_ERROR (CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder));
  }

  RETURN_ON_ERROR (CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder));
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplMemoryAllocationData (
  IN UNIVERSAL_PAYLOAD_MEMORY_ALLOCATION  *Data,
  IN UINTN                                Count
  )
{
  VOID           *ArrayEncoder;
  VOID           *SubMapEncoder;
  UINTN          Index;
  RETURN_STATUS  Status;

  RETURN_ON_ERROR (CborEncodeTextString (RootMapEncoderPointer, RESOURCEALLOCATION));
  RETURN_ON_ERROR (CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count));
  for (Index = 0; Index < Count; Index++) {
    RETURN_ON_ERROR (CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, NAME));
    RETURN_ON_ERROR (CborEncodeByteString (SubMapEncoder, (UINT8 *)&Data[Index].Name, sizeof (Data[Index].Name)));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, BASE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].MemoryBaseAddress));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, LENGTH));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].MemoryLength));
    RETURN_ON_ERROR (CborEncodeTextString (SubMapEncoder, TYPE));
    RETURN_ON_ERROR (CborEncodeUint64 (SubMapEncoder, Data[Index].MemoryType));
    RETURN_ON_ERROR (CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder));
  }

  RETURN_ON_ERROR (CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder));
  return EFI_SUCCESS;
}
