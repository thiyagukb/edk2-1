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
CborEncoder mapEncoder,arrayEncoder;
 UINT8   *buf;

VOID
Cbor_Create_Outermost_Container (
    VOID
)
{
  UINTN   size = 115;
  buf = AllocatePool(size);
  
  cbor_encoder_init(&gencoder, buf,115, 0);
  cbor_encoder_create_map(&gencoder, &mapEncoder, 4);
  DEBUG((DEBUG_ERROR,"KBT mapEncoder : %p \n",&mapEncoder));


  return;
}


VOID *
Cbor_Encoder_Set_Text_Strings (
  VOID * MapEncoder,
  const char * string
)
{
  DEBUG((DEBUG_ERROR,"KBT Cbor_Encoder_Set_Text_Strings MapEncoder : %p \n",MapEncoder));
  cbor_encode_text_stringz((CborEncoder *)MapEncoder, string);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_Byte_String (
    VOID * MapEncoder,
  const uint8_t * string,
  UINTN size
)
{
  cbor_encode_byte_string((CborEncoder *)MapEncoder, (const uint8_t *)string, size);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_UINT (
  VOID * MapEncoder,
  UINT64 Value
)
{
  DEBUG((DEBUG_ERROR,"KBT Cbor_Encoder_Set_UINT MapEncoder : %p \n",MapEncoder));
  cbor_encode_uint((CborEncoder *)MapEncoder, Value);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_INT (
  VOID * MapEncoder,
  UINT64 Value
)
{
  cbor_encode_int((CborEncoder *)MapEncoder, Value);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_Negative_INT (
  VOID * MapEncoder,
  UINT64 Absolute_Value
)
{
  cbor_encode_negative_int((CborEncoder *)MapEncoder, Absolute_Value);
  return MapEncoder;
}

VOID *
Cbor_Encoder_Set_BOOLEAN (
  VOID * MapEncoder,
  bool   Value
)
{
  cbor_encode_boolean((CborEncoder *)MapEncoder, Value);
  return MapEncoder;
}


VOID *
Init_Array (
  VOID *Parent,
  UINTN Length
)
{
  cbor_encoder_create_array(Parent,&arrayEncoder,Length);
  return (VOID*)&arrayEncoder;
}

VOID
Close_Array (
  VOID *Parent,
  VOID *Container
)
{
  cbor_encoder_close_container(Parent, Container);
}

VOID
SubMap_Init (
  VOID *Parent,
  VOID *Submap,
  UINTN Length
)
{
  cbor_encoder_create_map(Parent, Submap,Length);
}

VOID
End_Map (
  VOID *Parent,
  VOID *Container
)
{
  VOID   *Data;
  UINTN Size;

  if (Container == NULL){
    cbor_encoder_close_container(&gencoder,&mapEncoder);
      Size = cbor_encoder_get_buffer_size (&gencoder, (const uint8_t *)buf);
      DEBUG((DEBUG_ERROR,"KBT Size:%d \n",Size));
    Data =(VOID *) BuildGuidHob (&gProtoBufferGuid, Size);
  CopyMem(Data, buf, Size);
  }
  else {
    cbor_encoder_close_container(Parent, Container);
  }

}

PEI_CBOR_HANDLER_PPI   mPeiCborHandlerPpi = {
  Cbor_Encoder_Set_Text_Strings,
  Cbor_Encoder_Set_Byte_String,
  Cbor_Encoder_Set_UINT,
  Cbor_Encoder_Set_INT,
  Cbor_Encoder_Set_Negative_INT,
  Cbor_Encoder_Set_BOOLEAN,
  Init_Array,
  Close_Array,
  SubMap_Init,
  End_Map,
  (VOID *)&mapEncoder
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
