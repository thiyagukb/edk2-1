

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

VOID *
EFIAPI
CborDecoderGetRootMap (
   VOID                *Buffer,
   UINTN               Size
)
{
  CborParser Parser;
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
  //CborValue  Element;
  CborError ErrorType;

  SetMem(&Element,sizeof(Element),0);
  ErrorType = cbor_value_map_find_value (Map, String, &Element);

  if(ErrorType == 0) {
    //CopyMem(Value,&Element,sizeof(Element));
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
GetFromCborByteString (
  VOID                *Value,
  UINT8               *Result,
  UINTN               *Size
)
{
  CborError ErrorType;
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
GetFromCborTextString (
  VOID                *Value,
  UINT8               *Result,
  UINTN               *Sizen
)
{
  CborError ErrorType;
  CborValue next;
  ErrorType = cbor_value_copy_text_string	(	Value, (char *) Result, Sizen,  &next);

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

// Special APIs

EFI_STATUS
EFIAPI
GetfromCborUplExtraData (
  UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
  VOID                                     *Rootmap,
  UINTN                                    Count
)
{
  CborError ErrorType;
  CborValue UpldataValue;
  CborValue *UpldataSubmap,*ElementSubmap,next;
  UINTN length ,Base,Size;
  UINT8 buf[7];
  IoWrite8(0x3f8,'A');
  UpldataSubmap = (VOID *)CborDecoderFindValueInMap("UplExtradata",&RootValue);
  ErrorType = cbor_value_enter_container	(	UpldataSubmap,&UpldataValue );
 /* ErrorType = cbor_value_map_find_value(&RootValue,"UplExtradata",UpldataSubmap);
  ErrorType |= cbor_value_map_find_value (Rootmap, "UplExtradata", &element);*/

  DEBUG ((EFI_D_ERROR, " RootValue.type: %x \n", RootValue.type ));
  DEBUG ((EFI_D_ERROR, " RootValue.remaining: %x \n", RootValue.remaining ));

  IoWrite8(0x3f8,'C');
  if(ErrorType == 0) {
    IoWrite8(0x3f8,'D');
    ElementSubmap = CborDecoderFindValueInMap("Identifier",&UpldataValue);
    IoWrite8(0x3f8,'E');
    cbor_value_calculate_string_length(ElementSubmap, &length);
    IoWrite8(0x3f8,'F');
    GetFromCborTextString(ElementSubmap,buf,&length);
    cbor_value_copy_text_string	(	ElementSubmap, (char *) (UINT8 *) &buf, &length,  &next);
    IoWrite8(0x3f8,'G');
    CopyMem(Data->Identifier,buf,length);
    IoWrite8(0x3f8,'H');
  
    ElementSubmap = CborDecoderFindValueInMap("Base",&UpldataValue);
    IoWrite8(0x3f8,'I');
    CborDecoderGetUint64(ElementSubmap,&Base);
    IoWrite8(0x3f8,'J');
    Data->Base = Base;
  
    ElementSubmap = CborDecoderFindValueInMap("Size",&UpldataValue);
    IoWrite8(0x3f8,'K');
    CborDecoderGetUint64(ElementSubmap,&Size);
    IoWrite8(0x3f8,'L');
    Data->Size = Size;
    return EFI_SUCCESS;
  }
  else {
    IoWrite8(0x3f8,'M');
    return EFI_NOT_FOUND;
  }
}