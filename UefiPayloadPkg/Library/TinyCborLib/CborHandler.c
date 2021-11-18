/** @file
  ELF Load Image Support

Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/LoadFile.h>
#include "Proto.h"
#include "tinycbor\src\cbor.h"
#include <CborHandler.h>

CborEncoder gencoder;
CborEncoder mapEncoder,submapEncoder;
 UINT8   *buf;

VOID
Cbor_Create_Outermost_Container (
    VOID
)
{
  UINTN   size = 500;
  buf = AllocatePool(size);
  
  cbor_encoder_init(&gencoder, buf, 500, 0);
  return;
}

VOID *
Cbor_Encoder_Map_Init (
  VOID *ParentEncoder,
  IN UINTN   Length,
  BOOLEAN IsRoot
)
{
  if(IsRoot){
  cbor_encoder_create_map(&gencoder, &mapEncoder, Length);
  //return &mapEncoder;
  }
  else {
    cbor_encoder_create_map(ParentEncoder, &mapEncoder, Length);
  }
  return &mapEncoder;
}

VOID *
Cbor_Encoder_Set_Text_Strings (
  VOID * MapEncoder,
  const char * string
)
{
  cbor_encode_text_stringz(MapEncoder, string);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_Byte_String (
    VOID * MapEncoder,
  const uint8_t * string,
  UINTN size
)
{
  cbor_encode_byte_string(MapEncoder, (const uint8_t *)string, size);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_UINT (
  VOID * MapEncoder,
  UINT64 Value
)
{
  cbor_encode_uint(MapEncoder, Value);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_INT (
  VOID * MapEncoder,
  UINT64 Value
)
{
  cbor_encode_int(MapEncoder, Value);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_Negative_INT (
  VOID * MapEncoder,
  UINT64 Absolute_Value
)
{
  cbor_encode_negative_int(MapEncoder, Absolute_Value);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_BOOLEAN (
  VOID * MapEncoder,
  bool   Value
)
{
  cbor_encode_boolean(MapEncoder, Value);
  return MapEncoder;
}

VOID
Cbor_Encoder_Submap_End (
  VOID *Parent,
  VOID *Container
)
{
  VOID   *Data;
  UINTN Size;
  if (Container == NULL){
    cbor_encoder_close_container(&gencoder,Parent);
      Size = cbor_encoder_get_buffer_size (&gencoder, (const uint8_t *)buf);
    Data =(VOID *) BuildGuidHob (&gProtoBufferGuid, Size);
  CopyMem(Data, buf, Size);
  }
  else {
    cbor_encoder_close_container(Parent, Container);
  }

}

PEI_CBOR_HANDLER_PPI   mPeiCborHandlerPpi = {
  Cbor_Encoder_Map_Init,
  Cbor_Encoder_Set_Text_Strings,
  Cbor_Encoder_Set_Byte_String,
  Cbor_Encoder_Set_UINT,
  Cbor_Encoder_Set_INT,
  Cbor_Encoder_Set_Negative_INT,
  Cbor_Encoder_Set_BOOLEAN,
  Cbor_Encoder_Submap_End
};

EFI_PEI_PPI_DESCRIPTOR     gPpiCborHandlerPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiCborHandlerPpiGuid,
  &mPeiCborHandlerPpi
};

/**

  Install Pei Load File PPI.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The entry point executes successfully.
  @retval Others      Some error occurs during the execution of this function.

**/
EFI_STATUS
EFIAPI
CborHandlerEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;
  Cbor_Create_Outermost_Container();
  Status = PeiServicesInstallPpi (&gPpiCborHandlerPpiList);
  
  if(!EFI_ERROR (Status)){
    DEBUG((DEBUG_ERROR,"KBT CborHandlerEntryPoint done \n "));
  }
  return Status;
}
