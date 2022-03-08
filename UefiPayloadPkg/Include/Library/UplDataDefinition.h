/** @file
  Universal Payload data definition used by SetUplDataLib.h and GetUplDataLib.h

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UPL_DATA_DEFINITION_H__
#define __UPL_DATA_DEFINITION_H__

typedef struct {
  CHAR8                   Identifier[16];
  EFI_PHYSICAL_ADDRESS    Base;
  UINT64                  Size;
} UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA;

#endif
