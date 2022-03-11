

#include "tinycbor\src\cbor.h"
#include <Base.h>
#include <Uefi.h>
#include "Proto.h"
#include <Guid/CborRootmapHobGuid.h>
#include <Library/DebugLib.h>
#include <PiPei.h>
#include <Library/TinyCborEncoderLib.h>

CborEncoder RootEncoder ,RootMapEncoder,OuterEncoder;
VOID *Buffer;
CBOR_ROOTMAP_INFO *Data;

RETURN_STATUS
EFIAPI
TinyCborEncoderLibConstructor (
  VOID
)
{
  VOID              *Hob;
  UINTN   size;
  size  = 300;
  DEBUG ((DEBUG_INFO, "KBT In TinyCborEncoderLibConstructor.\n"));
  Hob = (VOID *)GetFirstGuidHob (&CborRootmapHobGuid);
  if(Hob == NULL) {
    Buffer = (VOID *)AllocatePool(size);
    cbor_encoder_init(&RootEncoder, Buffer, size, 0);

    //cbor_encoder_create_array(&OuterEncoder, &RootEncoder, 2);
    cbor_encoder_create_map(&RootEncoder, &RootMapEncoder, 5);
    Data = (CBOR_ROOTMAP_INFO *)BuildGuidHob (&CborRootmapHobGuid, sizeof(CBOR_ROOTMAP_INFO));
    Data->RootEncoder = &RootEncoder;
    Data->RootMapEncoder = &RootMapEncoder;
    DEBUG ((EFI_D_ERROR, "KBT If Root: %x Rootmap:%x \n",&RootEncoder,&RootMapEncoder ));
  }
  else {
    Data->RootEncoder = &RootEncoder;
    Data->RootMapEncoder = &RootMapEncoder;
    DEBUG ((EFI_D_ERROR, "KBT Else Root: %x Rootmap:%x \n",&RootEncoder,&RootMapEncoder ));
  }
  return RETURN_SUCCESS;
}

// Low level APIs
EFI_STATUS
EFIAPI
CborEncoderCreateSubMap (
  VOID *ParentEncoder,
  VOID *Child,
  UINTN Length             
)
{
  CborError ErrorType;
  ErrorType = cbor_encoder_create_map(ParentEncoder, Child, Length);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncoderCreateArray (
  VOID *ArrayEncoder,
  UINTN Length             
)
{
  CborError ErrorType;
  DEBUG ((EFI_D_ERROR, "KBT CborEncoderCreateArray \n" ));
  ErrorType = cbor_encoder_create_array	(&RootMapEncoder, ArrayEncoder, Length);
DEBUG ((EFI_D_ERROR, "KBT ErrorType: %d \n",ErrorType ));
  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeTextString (
  VOID *MapEncoder,
  const char *TextString
)
{
  CborError ErrorType;
  DEBUG ((EFI_D_ERROR, "KBT CborEncodeTextString \n" ));
  ErrorType = cbor_encode_text_stringz(MapEncoder, TextString);
  DEBUG ((EFI_D_ERROR, "KBT ErrorType: %d \n",ErrorType ));
  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeByteString (
  VOID          *MapEncoder,
  const UINT8   *ByteString,
  UINTN         size
)
{
  CborError ErrorType;
  ErrorType = cbor_encode_byte_string(MapEncoder, ByteString,size);
  
  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeUint64 (
  VOID *MapEncoder,
  UINT64 Value          
)
{
  CborError ErrorType;
  ErrorType = cbor_encode_uint(MapEncoder, Value);
  DEBUG ((DEBUG_INFO, "KBT CborEncodeUint64 %d\n",ErrorType));
  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeUint8 (
  VOID *MapEncoder,
  UINT8 Value          
)
{
  CborError ErrorType;
  ErrorType = cbor_encode_uint(MapEncoder, Value);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncodeBoolean (
  VOID    *MapEncoder,
  BOOLEAN Value          
)
{
  CborError ErrorType;
  ErrorType = cbor_encode_boolean(MapEncoder, Value);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborEncoderCloseContainer (
  VOID    *ParentEncoder ,
  VOID    *ContainerEncoder
)
{
  CborError ErrorType;
  if (ContainerEncoder != NULL) {
    
    ErrorType = cbor_encoder_close_container(ParentEncoder,ContainerEncoder);
    DEBUG ((DEBUG_INFO, "KBT CborEncoderCloseContainer1 %d\n",ErrorType));
  }
  else {
    
    ErrorType = cbor_encoder_close_container(&RootEncoder,&RootMapEncoder);
     DEBUG ((DEBUG_INFO, "KBT CborEncoderCloseContainer2 %d\n",ErrorType));
  }

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

// High Level APIs

EFI_STATUS
EFIAPI
SetToCborUint64 (
  const char            *Key,
  UINTN                 Value
)
{
  EFI_STATUS Status;
  DEBUG ((EFI_D_ERROR, "KBT SetToCborUint64 \n" ));
  Status = CborEncodeTextString(&RootMapEncoder,Key);
  if(Status) {
    return Status;
  }
  Status = CborEncodeUint64(&RootMapEncoder,Value);
  return Status;
}

EFI_STATUS
EFIAPI
SetToCborByteStrings (
  CHAR8                 *Key,
  UINT8                 *Value,
  UINTN                 size
)
{
  EFI_STATUS Status;
  Status = CborEncodeTextString(&RootMapEncoder,Key);
  if(Status) {
    return Status;
  }
  Status = CborEncodeByteString(&RootMapEncoder,Value,size);
  return Status;
}

EFI_STATUS
EFIAPI
SetToCborTextStrings (
  CHAR8                 *Key,
  UINT8                 *Value
)
{
  EFI_STATUS Status;
  Status = CborEncodeTextString(&RootMapEncoder,Key);
  if(Status) {
    return Status;
  }
  Status = CborEncodeTextString(&RootMapEncoder,(const char *)Value);
  return Status;
}

EFI_STATUS
EFIAPI
SetToCborBoolean (
  CHAR8                 *Key,
  BOOLEAN               Value
)
{
  EFI_STATUS Status;
  Status = CborEncodeTextString(&RootMapEncoder,Key);
  if(Status) {
    return Status;
  }
  Status = CborEncodeBoolean(&RootMapEncoder,Value);
  return Status;
}

// Spl APIs

EFI_STATUS
EFIAPI
CborGetBuffer (
  OUT VOID                **Buff,
  OUT UINTN               *Size
)
{

  UINTN used_size;
  used_size = cbor_encoder_get_buffer_size (&RootEncoder, (const uint8_t *)Buffer);
  DEBUG ((EFI_D_ERROR, "used_size: 0x%x \n", used_size ));
  *Buff = Buffer;
  *Size = used_size;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CloseCborRootMap (
  VOID
)
{
  EFI_STATUS Status;
  Status = CborEncoderCloseContainer(NULL,NULL);
  return Status;
}

EFI_STATUS
EFIAPI
SetToCborUplExtraData (
   UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
   UINTN                                    Count
)
{
  CborEncoder  sub_map_key;
  CborEncoder Arrayencoder;

  CborEncodeTextString(&RootMapEncoder, "UplExtradata");
  CborEncoderCreateArray(&Arrayencoder,Count);
  for (int i = 0;i<Count;i++) {
    CborEncoderCreateSubMap(&Arrayencoder,&sub_map_key,3);
    CborEncodeTextString(&sub_map_key,"Identifier");
    CborEncodeTextString(&sub_map_key,Data[i].Identifier);
    CborEncodeTextString(&sub_map_key,"Base");
    CborEncodeUint64(&sub_map_key,Data[i].Base);
    CborEncodeTextString(&sub_map_key,"Size");
    CborEncodeUint64(&sub_map_key,Data[i].Size);
    CborEncoderCloseContainer(&Arrayencoder,&sub_map_key);
    
  }
  CborEncoderCloseContainer(&RootMapEncoder,&Arrayencoder);
  return EFI_SUCCESS;
}