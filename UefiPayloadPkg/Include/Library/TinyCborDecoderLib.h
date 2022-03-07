/** @file
  Provides a service to retrieve a pointer to the start of HOB list.
  Only available to DXE module types.

  This library does not contain any functions or macros.  It simply exports a global
  pointer to the start of HOB list as defined in the Platform Initialization Driver
  Execution Environment Core Interface Specification.  The library constructor must
  initialize this global pointer to the start of HOB list, so it is available at the
  module's entry point.  Since there is overhead in looking up the pointer to the start
  of HOB list, only those modules that actually require access to the HOB list
  should use this library.

Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TINYCBOR_DECODER_LIB_H__
#define __TINYCBOR_DECODER_LIB_H__

typedef struct {
CHAR8                Identifier[16];
EFI_PHYSICAL_ADDRESS Base;
UINT64               Size;
} UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA;

VOID *
EFIAPI
CborDecoderGetRootMap (
   VOID                *Buffer,
   UINTN               Size
);

EFI_STATUS
EFIAPI
GetfromCborUplExtraData (
  UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
  VOID                                     *Rootmap,
  UINTN                                    Count
);

VOID *
EFIAPI
CborDecoderFindValueInMap (
  const char *String,
  VOID       *Map
);

EFI_STATUS
EFIAPI
CborDecoderGetUint64 (
  VOID                *Value,
  UINT64              *Result 
);

EFI_STATUS
EFIAPI
CborDecoderGetBoolean (
  VOID                *Value,
  BOOLEAN             *Result 
);

EFI_STATUS
EFIAPI
GetFromCborByteString (
  VOID                *Value,
  UINT8               *Result,
  UINTN               *Size
);

EFI_STATUS
EFIAPI
GetFromCborTextString (
  VOID                *Value,
  UINT8               *Result,
  UINTN               *Size
);

EFI_STATUS
EFIAPI
CborDecoderGetUint8 (
  VOID                *Value,
  UINT8               *Result 
);
#endif

