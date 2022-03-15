#include <Uefi.h>
#include <Guid/CborRootmapHobGuid.h>
#include <Library/DebugLib.h>
#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/GetUplDataLib.h>
#include "CborGetWrapper.h"

UPL_DATA_DECODER  RootValue;
CborParser        Parser;

RETURN_STATUS
EFIAPI
CborDecoderGetRootMap (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
{
  CborError  ErrorType;

  ErrorType = cbor_parser_init (Buffer, Size, 0, &Parser, &RootValue);
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetUint64 (
  IN  CHAR8   *String,
  OUT UINT64  *Result,
  IN  VOID    *Map
  )
{
  CborError  ErrorType;
  CborValue  Element;

  if (Map == NULL) {
    Map = &RootValue;
  }

  ErrorType = cbor_value_map_find_value (Map, String, &Element);
  ErrorType = cbor_value_get_uint64 (&Element, Result);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetUint8 (
  IN  CHAR8   *String,
  OUT UINT8  *Result,
  IN  VOID    *Map
  )
{
  CborError  ErrorType;
  CborValue  Element;

  if (Map == NULL) {
    Map = &RootValue;
  }

  ErrorType = cbor_value_map_find_value (Map, String, &Element);
  ErrorType = cbor_value_get_uint64 (&Element, (uint64_t *)Result);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetBoolean (
  IN  CHAR8    *String,
  OUT BOOLEAN  *Result,
  IN  VOID     *Map
  )
{
  CborError  ErrorType;
  CborValue  Element;

  if (Map == NULL) {
    Map = &RootValue;
  }

  ErrorType = cbor_value_map_find_value (Map, String, &Element);
  ErrorType = cbor_value_get_boolean (&Element, (bool *)Result);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetBinary (
  IN     VOID   *Value,
  OUT    VOID   *Result,
  IN OUT UINTN  *Size,
  IN     VOID   *Map
  )
{
  CborError  ErrorType;
  CborValue  Element;

  if (Map == NULL) {
    Map = &RootValue;
  }

  ErrorType = cbor_value_map_find_value (Map, Value, &Element);
  ErrorType = cbor_value_copy_byte_string (&Element, (uint8_t *)Result, Size, NULL);
  DEBUG ((DEBUG_INFO, "ErrorType %d.\n", ErrorType));
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetTextString (
  IN     VOID   *Value,
  OUT    UINT8  *Result,
  IN OUT UINTN  *Size,
  IN     VOID   *Map
  )
{
  CborError  ErrorType;
  CborValue  Element;

  if (Map == NULL) {
    Map = &RootValue;
  }

  ErrorType = cbor_value_map_find_value (Map, Value, &Element);
  ErrorType = cbor_value_copy_text_string (&Element, (char *)Result, Size, NULL);
  DEBUG ((DEBUG_INFO, "ErrorType %d.\n", ErrorType));
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetArrayNextMap (
  IN OUT UPL_DATA_DECODER  *NextMap
  )
{
  CborError  ErrorType;

  ErrorType = cbor_value_advance (NextMap);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

RETURN_STATUS
EFIAPI
CborDecoderGetArrayLengthAndFirstElement (
  IN     CHAR8             *String,
  OUT    UINTN             *Size,
  OUT    UPL_DATA_DECODER  *Map
  )
{
  CborError  ErrorType;

  CborValue  Array;

  ErrorType = cbor_value_map_find_value (&RootValue, String, &Array);
  ErrorType = cbor_value_get_array_length (&Array, Size);
  ErrorType = cbor_value_enter_container (&Array, Map);
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}
