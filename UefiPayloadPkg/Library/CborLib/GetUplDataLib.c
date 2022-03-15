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
  IN OUT UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
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

VOID
EFIAPI
GetCbor (
  OUT VOID   *Buffer,
  OUT UINTN  Size
  )
{
  CborDecoderGetRootMap (Buffer, Size);

  UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  Data[2];
  UINTN                                    Count = 2;
  UINTN                                    Index = 0;
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_INFO   Data1;
  UINTN                                    Count1 = 1;
  UINT8 buf[16];
  UINTN size1 = 16;
  BOOLEAN Res;
  UINT8 Res8;
GetUplBinary("MyGuid",&buf,&size1);
DEBUG ((DEBUG_INFO, "KBT MyGuid: %g\n",buf));
GetUplBoolean("UseMmio",&Res);
DEBUG ((DEBUG_INFO, "KBT UseMmio: %x\n",Res));
GetUplUint8("Test",&Res8);
DEBUG ((DEBUG_INFO, "KBT Test: %x\n",Res8));

  GetUplExtraData (
    Data,
    &Count,
    Index
    );
  DEBUG ((EFI_D_ERROR, "Identifier: 0x%a \n", (CHAR8 *)&Data[1].Identifier));
  DEBUG ((EFI_D_ERROR, "Identifier: 0x%a \n", (CHAR8 *)&Data[0].Identifier));

  GetUplPciRootBridges (
    &Data1,
    &Count1,
    Index
  );

}
