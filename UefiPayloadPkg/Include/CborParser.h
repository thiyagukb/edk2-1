/** @file
  Load image file from fv to memory.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Revision Reference:
  This PPI is introduced in PI Version 1.0.

**/

#ifndef __CBOR_PARSER__
#define __CBOR_PARSER__

VOID *
Cbor_Parser_Init (
  OUT VOID                *Buffer,
  OUT UINTN               Size
  );

VOID
Cbor_Get_Byte_by_String (
  IN VOID* Cborval,
  IN char * InputString,
  IN UINTN size,
  OUT UINT8* Buffer
);

UINT64
Cbor_Get_Uint64_by_String (
  IN VOID* Cborval,
  IN char * InputString
);

VOID *
Cbor_Get_Submap_by_String (
  IN VOID* Rootmap,
  IN char * InputString
);

INT64
Cbor_Get_int64_by_String (
  IN VOID* Cborval,
  IN char * InputString
);

int
Cbor_Get_int_by_String (
  IN VOID* Cborval,
  IN char * InputString
);
#endif
