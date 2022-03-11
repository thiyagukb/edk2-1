

#include "tinycbor\src\cbor.h"
#include <Base.h>
#include <Uefi.h>
#include "Proto.h"
#include <Guid/CborRootmapHobGuid.h>
#include <Library/DebugLib.h>
#include <PiPei.h>
#include <Library/TinyCborDecoderLib.h>
#include <Library/IoLib.h>


CborValue RootValue,Element;
CborParser Parser;

VOID *
EFIAPI
CborDecoderGetRootMap (
   VOID                *Buffer,
   UINTN               Size
)
{
  
  CborError ErrorType;
  ErrorType = cbor_parser_init(Buffer, Size, 0, &Parser, &RootValue);
  
  if(ErrorType == 0) {
    return &RootValue;
  }
  else {
    return NULL;
  }
}

VOID *
EFIAPI
CborDecoderFindValueInMap (
  const char *String,
  VOID       *Map
)
{
  CborError ErrorType;

  SetMem(&Element,sizeof(Element),0);
  ErrorType = cbor_value_map_find_value (Map, String, &Element);

  if(ErrorType == 0) {
    return &Element;
  }
  else {
    return NULL;
  }
}

EFI_STATUS
EFIAPI
CborDecoderGetUint64 (
  VOID                *Value,
  UINT64              *Result 
)
{
  CborError ErrorType;
  ErrorType = cbor_value_get_uint64(Value, Result);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborDecoderGetBoolean (
  VOID                *Value,
  BOOLEAN             *Result 
)
{
  CborError ErrorType;
  ErrorType = cbor_value_get_boolean(Value, Result);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborDecoderGetByteString (
  VOID                *Value,
  UINT8               *Result,
  UINTN               *Size
)
{
  CborError ErrorType;
  cbor_value_calculate_string_length(Value,Size);
  ErrorType = cbor_value_copy_byte_string (Value,  Result, Size, NULL);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborDecoderGetTextString (
  VOID                *Value,
  UINT8               *Result,
  UINTN               *Size
)
{
  CborError ErrorType;
  CborValue next;
  cbor_value_calculate_string_length(Value,Size);
  ErrorType = cbor_value_copy_text_string	(	Value, (char *) Result, Size,  &next);
  
  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

EFI_STATUS
EFIAPI
CborDecoderGetUint8 (
  VOID                *Value,
  UINT8               *Result 
)
{
  CborError ErrorType;
  ErrorType = cbor_value_get_int(Value,(int *) Result);

  if(ErrorType == 0) {
    return EFI_SUCCESS;
  }
  else {
    return EFI_NOT_FOUND;
  }
}

// High Level APIs

VOID *
EFIAPI
GetFromCborRootMapInit (
  VOID                 *Buffer,
  UINTN                Size

)
{
  VOID *RootencoderPointer;
  RootencoderPointer = CborDecoderGetRootMap(Buffer,Size);
  return RootencoderPointer;
}

VOID *
EFIAPI
GetFromCborValueInMap (
  const char *String,
  VOID       *Map
)
{
  CborValue *ElementPointer;
  ElementPointer = CborDecoderFindValueInMap(String,Map);
  return ElementPointer;
}
EFI_STATUS
EFIAPI
GetFromCborUint64 (
  VOID   *Key,
  UINT64 *Result
)
{
  EFI_STATUS Status;
  Status = CborDecoderGetUint64(Key,Result);
  return Status;
}

EFI_STATUS
EFIAPI
GetFromCborUint8 (
  VOID   *Key,
  UINT8 *Result
)
{
  EFI_STATUS Status;
  Status = CborDecoderGetUint8(Key,Result);
  return Status;
}

EFI_STATUS
EFIAPI
GetFromCborBoolean (
  VOID    *Key,
  BOOLEAN *Result
)
{
  EFI_STATUS Status;
  Status = CborDecoderGetBoolean(Key,Result);
  return Status;
}

EFI_STATUS
EFIAPI
GetFromCborTextString (
  VOID    *Key,
  UINT8   *Result,
  UINTN   *Size
)
{
  EFI_STATUS Status;
  Status = CborDecoderGetTextString(Key,Result,Size);
  return Status;
}

EFI_STATUS
EFIAPI
GetFromCborByteString (
  VOID    *Key,
  BOOLEAN *Result,
  UINTN   *Size
)
{
  EFI_STATUS Status;
  Status = CborDecoderGetByteString(Key,Result,Size);
  return Status;
}

// Special APIs
CborValue UpldataValue;
EFI_STATUS
EFIAPI
GetfromCborUplExtraData (
  UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
  VOID                                     *Rootmap,
  UINTN                                    Count
)
{
  CborError ErrorType;

  CborValue UpldataSubmap,*ElementSubmap,next;
  UINTN length ,Base,Size;
  UINT8 buf[7];
  CborError err;

  cbor_value_map_find_value (&RootValue, "UplExtradata", &UpldataSubmap);
  ErrorType = cbor_value_enter_container	(	&UpldataSubmap,&UpldataValue );

  if(ErrorType == 0) {
    
    if(Count > 1) {
      for (int i = 0;i<Count;i++) {
        ElementSubmap = CborDecoderFindValueInMap("Identifier",&UpldataValue);
        GetFromCborTextString(ElementSubmap,buf,&length);
        cbor_value_copy_text_string	(	ElementSubmap, (char *) (UINT8 *) &buf, &length,  &next);
        CopyMem(Data[i].Identifier,buf,length);

        ElementSubmap = CborDecoderFindValueInMap("Base",&UpldataValue);
        CborDecoderGetUint64(ElementSubmap,&Base);
        Data[i].Base = Base;

        ElementSubmap = CborDecoderFindValueInMap("Size",&UpldataValue);
        CborDecoderGetUint64(ElementSubmap,&Size);
        Data[i].Size = Size;

        err = cbor_value_advance(&UpldataValue);
      }
    }
    else {
      ElementSubmap = CborDecoderFindValueInMap("Identifier",&UpldataValue);
      GetFromCborTextString(ElementSubmap,buf,&length);
      cbor_value_copy_text_string	(	ElementSubmap, (char *) (UINT8 *) &buf, &length,  &next);
      CopyMem(Data->Identifier,buf,length);

      ElementSubmap = CborDecoderFindValueInMap("Base",&UpldataValue);
      CborDecoderGetUint64(ElementSubmap,&Base);
      Data->Base = Base;

      ElementSubmap = CborDecoderFindValueInMap("Size",&UpldataValue);
      CborDecoderGetUint64(ElementSubmap,&Size);
      Data->Size = Size;
    }
    return EFI_SUCCESS;
  }
  else {

    return EFI_NOT_FOUND;
  }
}