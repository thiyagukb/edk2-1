/** @file
  Load image file from fv to memory.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  This PPI is introduced in PI Version 1.0.

**/

#ifndef __CBOR_HANDLER__
#define __CBOR_HANDLER__

typedef struct _PEI_CBOR_HANDLER_PPI PEI_CBOR_HANDLER_PPI;


typedef
VOID *
(*PEI_CBOR_CREATE_NEW_ENCODER_MAP) (
  IN UINT8   *buf,
  IN UINTN   Length
);

typedef
VOID *
(*PEI_CBOR_SET_TEXT_STRING) (
  VOID * MapEncoder,
  const char * string
);


typedef
VOID *
(*PEI_CBOR_SET_BYTE_STRING) (
    VOID * MapEncoder,
  const UINT8 * string,
  UINTN size
);

typedef
VOID *
(*PEI_CBOR_SET_UINT) (
  VOID * MapEncoder,
  UINT64 Value
);

typedef
VOID *
(*PEI_CBOR_SET_INT) (
  VOID * MapEncoder,
  UINT64 Value
);

typedef
VOID *
(*PEI_CBOR_SET_NEGATIVE_INT) (
  VOID * MapEncoder,
  UINT64 Absolute_Value
);

typedef
VOID *
(*PEI_CBOR_SET_BOOLEAN) (
  VOID * MapEncoder,
  BOOLEAN   Value
);

typedef
VOID
(*PEI_CBOR_CLOSE_ARRAY) (
  VOID *Parent,
  VOID *ArrayEncoder  
);

typedef
VOID*
(*PEI_CBOR_INIT_ARRAY) (
  VOID *Parent,
  UINTN Length
);

typedef
VOID
(*PEI_CBOR_SUBMAP_INIT) (
  VOID *Parent,
  VOID *Submap,
  UINTN Length  
);

typedef
VOID
(*PEI_CBOR_CLOSE_MAP) (
  VOID *Parent,
  VOID *Container
);
/*
typedef
CborError
(*PEI_CBOR_ENCODER_CREATE_MAP)(
  CborEncoder *parentEncoder,
  CborEncoder *mapEncoder,
  size_t length
);

typedef
CborError
(*PEI_CBOR_ENCODER_TEXT_STRING)(
  CborEncoder *encoder,
  const char *string,
  size_t length
);

typedef
CborError
(*PEI_CBOR_ENCODER_BYTE_STRING)(
  CborEncoder *encoder,
  const uint8_t *string,
  size_t length
);

typedef
CborError
(*PEI_CBOR_ENCODER_UINT)(
  CborEncoder *encoder,
  uint64_t value
);


typedef
CborError
(*PEI_CBOR_CLOSE_CONTAINER)(
  CborEncoder *parentEncoder,
  const CborEncoder *containerEncoder
);

typedef
CborError
(*PEI_CBOR_VALUE_MAP_FIND_VALUE)(
  const CborValue *map,
  const char *string,
  CborValue *element
);

typedef
CborError
(*PEI_CBOR_VALUE_COPY_STRING)(
  const CborValue *value,
  void *buffer,
  size_t *buflen,
  CborValue *next
);

typedef
uint64_t
(*PEI_CBOR_VALUE_DECODE_INT64)(
  const CborValue *value
);
*/

///
/// This PPI is a pointer to the Load File service.
/// This service will be published by a PEIM. The PEI Foundation
/// will use this service to launch the known PEI module images.
///
struct _PEI_CBOR_HANDLER_PPI {
  PEI_CBOR_SET_TEXT_STRING Set_Text_Strings;
  PEI_CBOR_SET_BYTE_STRING Set_Byte_Strings;
  PEI_CBOR_SET_UINT    Set_UINT;
  PEI_CBOR_SET_INT Set_INT;
  PEI_CBOR_SET_NEGATIVE_INT Set_Negative_INT;
  PEI_CBOR_SET_BOOLEAN Set_BOOLEAN;
  PEI_CBOR_INIT_ARRAY Init_Array;
  PEI_CBOR_CLOSE_ARRAY Close_Array;
  PEI_CBOR_SUBMAP_INIT SubMap_Init;
  PEI_CBOR_CLOSE_MAP Close_Map;
  VOID *gEncoder;
};

extern EFI_GUID gPeiCborHandlerPpiGuid;

#endif
