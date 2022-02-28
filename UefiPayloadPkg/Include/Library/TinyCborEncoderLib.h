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

#ifndef __TINYCBOR_ENCODER_LIB_H__
#define __TINYCBOR_ENCODER_LIB_H__

typedef struct {
CHAR8                Identifier[16];
EFI_PHYSICAL_ADDRESS Base;
UINT64               Size;
} UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA;


EFI_STATUS
EFIAPI
SetToCborUint64 (
  const char            *Key,
  UINTN                 Value
);

EFI_STATUS
EFIAPI
SetToCborByteStrings (
  CHAR8                 *Key,
  UINT8                 *Value,
  UINTN                 Size
);

EFI_STATUS
EFIAPI
SetToCborTextStrings (
  CHAR8                 *Key,
  UINT8                 *Value
);

EFI_STATUS
EFIAPI
SetToCborBoolean (
  CHAR8                 *Key,
  BOOLEAN               Value
);

EFI_STATUS
EFIAPI
CloseCborRootMap (
  VOID 
);

EFI_STATUS
EFIAPI
CborEncoderCloseContainer (
  VOID    *ParentEncoder ,
  VOID    *ContainerEncoder
);

EFI_STATUS
EFIAPI
CborGetBuffer (
  OUT VOID                **Buff,
  OUT UINTN               *Size
);

EFI_STATUS
EFIAPI
SetToCborUplExtraData (
   UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA  *Data,
   UINTN                                    Count
);

#endif

