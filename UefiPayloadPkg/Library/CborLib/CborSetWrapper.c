#include "tinycbor\src\cbor.h"
#include <Base.h>
#include <Library/DebugLib.h>
#include <PiPei.h>
#include <Library/MemoryAllocationLib.h>

CborEncoder  mSubMapEncoder;
CborEncoder  mArrayEncoder;

CborEncoder  *gRootMapEncoderPointer;

// Low level APIs
VOID
CborEncoderInit (
  OUT VOID  **RootEncoderPointer,
  OUT VOID  **RootMapEncoderPointer,
  OUT VOID  **Buffer,
  IN UINTN  size
  )
{
  *RootEncoderPointer    = (VOID *)AllocatePool (size + 2*sizeof (CborEncoder));
  *RootMapEncoderPointer = (UINT8 *)*RootEncoderPointer + sizeof (CborEncoder);
  *Buffer                = (UINT8 *)*RootEncoderPointer + 2*sizeof (CborEncoder);

  cbor_encoder_init (*RootEncoderPointer, *Buffer, size, 0);
  cbor_encoder_create_map (*RootEncoderPointer, *RootMapEncoderPointer, CborIndefiniteLength);
}

EFI_STATUS
EFIAPI
CborEncoderCreateSubMap (
  VOID  *ParentEncoder,
  VOID  **Child
  )
{
  CborError  ErrorType;

  ErrorType = cbor_encoder_create_map (ParentEncoder, &mSubMapEncoder, CborIndefiniteLength);
  *Child    = &mSubMapEncoder;

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncoderCreateArray (
  VOID   *ParentEncoder,
  VOID   **ArrayEncoder,
  UINTN  Length
  )
{
  CborError  ErrorType;

  DEBUG ((EFI_D_ERROR, "KBT CborEncoderCreateArray \n"));
  ErrorType     = cbor_encoder_create_array (ParentEncoder, &mArrayEncoder, Length);
  *ArrayEncoder = &mArrayEncoder;
  DEBUG ((EFI_D_ERROR, "KBT ErrorType: %d \n", ErrorType));
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeTextString (
  VOID         *MapEncoder,
  const CHAR8  *TextString
  )
{
  CborError  ErrorType;

  ErrorType = cbor_encode_text_stringz (MapEncoder, TextString);
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeByteString (
  VOID         *MapEncoder,
  const UINT8  *ByteString,
  UINTN        size
  )
{
  CborError  ErrorType;

  ErrorType = cbor_encode_byte_string (MapEncoder, ByteString, size);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeUint64 (
  VOID    *MapEncoder,
  UINT64  Value
  )
{
  CborError  ErrorType;

  ErrorType = cbor_encode_uint (MapEncoder, Value);
  DEBUG ((DEBUG_INFO, "KBT CborEncodeUint64 %d\n", ErrorType));
  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeUint8 (
  VOID   *MapEncoder,
  UINT8  Value
  )
{
  CborError  ErrorType;

  ErrorType = cbor_encode_uint (MapEncoder, Value);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeBoolean (
  VOID     *MapEncoder,
  BOOLEAN  Value
  )
{
  CborError  ErrorType;

  ErrorType = cbor_encode_boolean (MapEncoder, Value);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncoderCloseContainer (
  VOID  *ParentEncoder,
  VOID  *ContainerEncoder
  )
{
  CborError    ErrorType;

  ErrorType = cbor_encoder_close_container (ParentEncoder, ContainerEncoder);

  if (ErrorType == 0) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

UINTN
EFIAPI
CborGetBuffer (
  VOID  *ParentEncoder,
  VOID  *Buffer
  )
{
  UINTN  used_size;

  used_size = cbor_encoder_get_buffer_size (ParentEncoder, (const uint8_t *)Buffer);
  DEBUG ((EFI_D_ERROR, "used_size: 0x%x \n", used_size));

  return used_size;
}
