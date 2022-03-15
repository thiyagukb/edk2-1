#include <PiPei.h>
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/CborRootmapHobGuid.h>
#include "CborSetWrapper.h"
#include <Library/SetUplDataLib.h>

#define ROW_LIMITER  16

VOID  *RootEncoderPointer, *RootMapEncoderPointer;
VOID  *Buffer;

RETURN_STATUS
EFIAPI
SetUplDataLibConstructor (
  VOID
  )
{
  VOID               *Hob;
  UINTN              size;
  CBOR_ROOTMAP_INFO  *Data;

  size = 2000;
  DEBUG ((DEBUG_INFO, "KBT In TinyCborEncoderLibConstructor.\n"));
  Hob = (VOID *)GetFirstGuidHob (&CborRootmapHobGuid);
  if (Hob == NULL) {
    CborEncoderInit (&RootEncoderPointer, &RootMapEncoderPointer, &Buffer, size);
    Data                 = (CBOR_ROOTMAP_INFO *)BuildGuidHob (&CborRootmapHobGuid, sizeof (CBOR_ROOTMAP_INFO));
    Data->RootEncoder    = RootEncoderPointer;
    Data->RootMapEncoder = RootMapEncoderPointer;
    Data->Buffer         = Buffer;
  } else {
    Data                  = GET_GUID_HOB_DATA (Hob);
    RootEncoderPointer    = Data->RootEncoder;
    RootMapEncoderPointer = Data->RootMapEncoder;
    Buffer                = Data->Buffer;
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

  DEBUG ((EFI_D_ERROR, "KBT SetUplUint64 \n"));
  Status = CborEncodeTextString (RootMapEncoderPointer, Key);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = CborEncodeUint64 (RootMapEncoderPointer, Value);
  return Status;
}

RETURN_STATUS
EFIAPI
SetUplUint8 (
  IN CHAR8  *Key,
  IN UINT8  Value
  )
{
  RETURN_STATUS  Status;

  Status = CborEncodeTextString (RootMapEncoderPointer, Key);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = CborEncodeUint8 (RootMapEncoderPointer, Value);
  return Status;
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

  Status = CborEncodeTextString (RootMapEncoderPointer, Key);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = CborEncodeByteString (RootMapEncoderPointer, Value, Size);
  return Status;
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

  Status = CborEncodeTextString (RootMapEncoderPointer, Key);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = CborEncodeTextString (RootMapEncoderPointer, (const char *)Value);
  return Status;
}

RETURN_STATUS
EFIAPI
SetUplBoolean (
  IN CHAR8    *Key,
  IN BOOLEAN  Value
  )
{
  RETURN_STATUS  Status;

  Status = CborEncodeTextString (RootMapEncoderPointer, Key);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = CborEncodeBoolean (RootMapEncoderPointer, Value);
  return Status;
}

RETURN_STATUS
EFIAPI
LockUplAndGetBuffer (
  OUT VOID   **Buff,
  OUT UINTN  *Size
  )
{
  RETURN_STATUS  Status;

  Status = CborEncoderCloseContainer (RootEncoderPointer, RootMapEncoderPointer);
  *Size  = CborGetBuffer (RootEncoderPointer, Buffer);
  *Buff  = Buffer;
  return Status;
}

RETURN_STATUS
EFIAPI
SetUplExtraData (
  IN UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
  IN UINTN                                    Count
  )
{
  VOID   *ArrayEncoder;
  VOID   *SubMapEncoder;
  UINTN  Index;

  CborEncodeTextString (RootMapEncoderPointer, "UplExtradata");
  CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count);
  for (Index = 0; Index < Count; Index++) {
    CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder);
    CborEncodeTextString (SubMapEncoder, "Identifier");
    CborEncodeTextString (SubMapEncoder, Data[Index].Identifier);
    CborEncodeTextString (SubMapEncoder, "Base");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Base);
    CborEncodeTextString (SubMapEncoder, "Size");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Size);
    CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder);
  }

  CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder);
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplMemoryMap (
  IN EFI_MEMORY_DESCRIPTOR  *Data,
  IN UINTN                  Count
  )
{
  VOID   *ArrayEncoder;
  VOID   *SubMapEncoder;
  UINTN  Index;

  CborEncodeTextString (RootMapEncoderPointer, "MemoryMap");
  CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count);
  for (Index = 0; Index < Count; Index++) {
    CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder);
    CborEncodeTextString (SubMapEncoder, "Base");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PhysicalStart);
    CborEncodeTextString (SubMapEncoder, "NumberOfPages");
    CborEncodeUint64 (SubMapEncoder, Data[Index].NumberOfPages);
    CborEncodeTextString (SubMapEncoder, "Type");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Type);
    CborEncodeTextString (SubMapEncoder, "Attribute");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Attribute);
    CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder);
  }

  CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder);
  return EFI_SUCCESS;
}

UINT8  buf11[16];

UINTN
PrintHex1 (
  IN  UINT8   *DataStart,
  IN  UINT16  DataSize
  )
{
  UINTN  Index1;
  UINTN  Index2;
  UINT8  *StartAddr;

  StartAddr = DataStart;
  for (Index1 = 0; Index1 * ROW_LIMITER < DataSize; Index1++) {
    DEBUG ((DEBUG_ERROR, "   0x%04p:", (DataStart - StartAddr)));
    for (Index2 = 0; (Index2 < ROW_LIMITER) && (Index1 * ROW_LIMITER + Index2 < DataSize); Index2++) {
      DEBUG ((DEBUG_ERROR, " %02x", *DataStart));
      DataStart++;
    }

    DEBUG ((DEBUG_ERROR, "\n"));
  }

  return 0;
}



RETURN_STATUS
EFIAPI
SetUplPciRootBridges (
  IN UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_INFO  *Data,
  IN UINTN                                    Count
  )
{
  VOID   *ArrayEncoder;
  VOID   *SubMapEncoder;
  UINTN  Index;


  CborEncodeTextString (RootMapEncoderPointer, "RootBridgeInfo");
  CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count);

  for (Index = 0; Index < Count; Index++) {
    CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder);

    CborEncodeTextString (SubMapEncoder, "Segment");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Segment);

    CborEncodeTextString (SubMapEncoder, "Supports");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Supports);

    CborEncodeTextString (SubMapEncoder, "Attributes");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Attributes);

    CborEncodeTextString (SubMapEncoder, "DmaAbove4G");
    CborEncodeBoolean (SubMapEncoder, Data[Index].DmaAbove4G);

    CborEncodeTextString (SubMapEncoder, "NoExtendedConfigSpace");
    CborEncodeBoolean (SubMapEncoder, Data[Index].NoExtendedConfigSpace);

    CborEncodeTextString (SubMapEncoder, "AllocationAttributes");
    CborEncodeUint64 (SubMapEncoder, Data[Index].AllocationAttributes);

    CborEncodeTextString (SubMapEncoder, "BusBase");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Bus.Base);

    CborEncodeTextString (SubMapEncoder, "BusLimit");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Bus.Limit);

    CborEncodeTextString (SubMapEncoder, "BusTranslation");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Bus.Translation );

    CborEncodeTextString (SubMapEncoder, "IoBase");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Io.Base);

    CborEncodeTextString (SubMapEncoder, "IoLimit");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Io.Limit);

    CborEncodeTextString (SubMapEncoder, "IoTranslation");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Io.Translation);

    CborEncodeTextString (SubMapEncoder, "MemBase");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Mem.Base);

    CborEncodeTextString (SubMapEncoder, "MemLimit");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Mem.Limit);

    CborEncodeTextString (SubMapEncoder, "MemTranslation");
    CborEncodeUint64 (SubMapEncoder, Data[Index].Mem.Translation);

    CborEncodeTextString (SubMapEncoder, "MemAbove4GBase");
    CborEncodeUint64 (SubMapEncoder, Data[Index].MemAbove4G.Base);

    CborEncodeTextString (SubMapEncoder, "MemAbove4GLimit");
    CborEncodeUint64 (SubMapEncoder, Data[Index].MemAbove4G.Limit);

    CborEncodeTextString (SubMapEncoder, "MemAbove4GTranslation");
    CborEncodeUint64 (SubMapEncoder, Data[Index].MemAbove4G.Translation);

    CborEncodeTextString (SubMapEncoder, "PMemBase");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PMem.Base);

    CborEncodeTextString (SubMapEncoder, "PMemLimit");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PMem.Limit);

    CborEncodeTextString (SubMapEncoder, "PMemTranslation");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PMem.Translation);

    CborEncodeTextString (SubMapEncoder, "PMemAbove4GBase");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PMemAbove4G.Base);

    CborEncodeTextString (SubMapEncoder, "PMemAbove4GLimit");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PMemAbove4G.Limit);

    CborEncodeTextString (SubMapEncoder, "PMemAbove4GTranslation");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PMemAbove4G.Translation);

    CborEncodeTextString (SubMapEncoder, "HID");
    CborEncodeUint64 (SubMapEncoder, Data[Index].HID);

    CborEncodeTextString (SubMapEncoder, "UID");
    CborEncodeUint64 (SubMapEncoder, Data[Index].UID);

    CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder);
  }
  CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder);
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplResourceData (
  IN EFI_HOB_RESOURCE_DESCRIPTOR_DATA         *Data,
  IN UINTN                                    Count
  )
{
  VOID   *ArrayEncoder;
  VOID   *SubMapEncoder;
  UINTN  Index;

  DEBUG ((EFI_D_ERROR, "KBT Data->Owner: %g \n", &Data->Owner));
  DEBUG ((EFI_D_ERROR, "KBT sizeof(Data[Index].Owner: %x \n", sizeof(Data[0].Owner)));
  CborEncodeTextString (RootMapEncoderPointer, "Resource");
  CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count);
  for (Index = 0; Index < Count; Index++) {
    CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder);
    CborEncodeTextString (SubMapEncoder, "Owner");
    CborEncodeByteString (SubMapEncoder, (UINT8 *)&Data[Index].Owner,sizeof(Data[Index].Owner));
    CborEncodeTextString (SubMapEncoder, "Type");
    CborEncodeUint64 (SubMapEncoder, Data[Index].ResourceType);
    CborEncodeTextString (SubMapEncoder, "Attribute");
    CborEncodeUint64 (SubMapEncoder, Data[Index].ResourceAttribute);
    CborEncodeTextString (SubMapEncoder, "Base");
    CborEncodeUint64 (SubMapEncoder, Data[Index].PhysicalStart);
    CborEncodeTextString (SubMapEncoder, "Length");
    CborEncodeUint64 (SubMapEncoder, Data[Index].ResourceLength);
    CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder);
  }

  CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder);
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetUplMemoryAllocationData (
  IN EFI_HOB_MEMORY_ALLOCATION_DATA           *Data,
  IN UINTN                                    Count
  )
{
  VOID   *ArrayEncoder;
  VOID   *SubMapEncoder;
  UINTN  Index;

  CborEncodeTextString (RootMapEncoderPointer, "ResourceAllocation");
  CborEncoderCreateArray (RootMapEncoderPointer, &ArrayEncoder, Count);
  for (Index = 0; Index < Count; Index++) {
    CborEncoderCreateSubMap (ArrayEncoder, &SubMapEncoder);
    CborEncodeTextString (SubMapEncoder, "Name");
    CborEncodeByteString (SubMapEncoder, (UINT8 *)&Data[Index].Name,sizeof(Data[Index].Name));
    CborEncodeTextString (SubMapEncoder, "Base");
    CborEncodeUint64 (SubMapEncoder, Data[Index].MemoryBaseAddress);
    CborEncodeTextString (SubMapEncoder, "Length");
    CborEncodeUint64 (SubMapEncoder, Data[Index].MemoryLength);
    CborEncodeTextString (SubMapEncoder, "Type");
    CborEncodeUint64 (SubMapEncoder, Data[Index].MemoryType);
    CborEncoderCloseContainer (ArrayEncoder, SubMapEncoder);
  }

  CborEncoderCloseContainer (RootMapEncoderPointer, ArrayEncoder);
  return EFI_SUCCESS;
}

RETURN_STATUS
EFIAPI
SetCbor (
  OUT VOID   **Buffer,
  OUT UINTN  *Size
  )
{
  UINTN  size;
  UINTN  used_size;

  size = 300;
  VOID  *buf = AllocatePool (size);

  UINT32  buffer1[] = { 0x12345678, 0x0, 0x90ABCDEF };

  SetUplBinary ("MyGuid", (UINT8 *)&gUefiPayloadPkgTokenSpaceGuid, 16);
  SetUplBinary ("RawByte", (UINT8 *)buffer1, 12);

  SetUplUint64 ("BaudRate", PcdGet32 (PcdSerialBaudRate));
  SetUplUint64 ("RegisterBase", PcdGet64 (PcdSerialRegisterBase));
  SetUplUint64 ("RegisterStride", PcdGet32 (PcdSerialRegisterStride));

  SetUplBoolean ("UseMmio", (UINTN)PcdGetBool (PcdSerialUseMmio));
  SetUplUint8("Test",0xA);
  // SetUplUint64 ("BaudRate", PcdGet32 (PcdSerialBaudRate));
  UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  Extra[3];

  Extra[0].Base = 12;
  char  idnt[] = "hello\0";

  AsciiStrCpyS (Extra[0].Identifier, sizeof (idnt), idnt);

  Extra[1].Base = 120;
  char  idnt1[] = "world\0";

  AsciiStrCpyS (Extra[1].Identifier, sizeof (idnt), idnt1);

  Extra[2].Base = 102;
  char  idnt2[] = "you\0";

  AsciiStrCpyS (Extra[2].Identifier, sizeof (idnt), idnt2);

  SetUplExtraData (Extra, 3);
  LockUplAndGetBuffer (&buf, &used_size);

  DEBUG ((EFI_D_ERROR, "used_size: 0x%x \n", used_size));
  PrintHex1 (buf, used_size);

  *Buffer = buf;
  *Size   = used_size;

  return 0;
}
