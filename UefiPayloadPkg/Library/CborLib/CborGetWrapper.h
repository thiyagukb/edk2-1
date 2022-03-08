/** @file


Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __CBOR_GET_WRAPPER_H__
#define __CBOR_GET_WRAPPER_H__

#include "tinycbor\src\cbor.h"
#include  <Base.h>
typedef CborValue UPL_DATA_DECODER;

RETURN_STATUS
EFIAPI
CborDecoderGetRootMap (
  VOID   *Buffer,
  UINTN  Size
  );

RETURN_STATUS
EFIAPI
CborDecoderGetUint64 (
  IN  CHAR8   *String,
  OUT UINT64  *Result,
  IN  VOID    *Map
  );

RETURN_STATUS
EFIAPI
CborDecoderGetTextString (
  IN     VOID   *Value,
  OUT    UINT8  *Result,
  IN OUT UINTN  *Size,
  IN     VOID   *Map
  );

RETURN_STATUS
EFIAPI
CborDecoderGetArrayNextMap (
  IN OUT UPL_DATA_DECODER  *NextMap
  );


RETURN_STATUS
EFIAPI
CborDecoderGetArrayLengthAndFirstElement (
  IN     CHAR8             *String,
  OUT    UINTN             *Size,
  OUT    UPL_DATA_DECODER  *Map
  );

#endif
