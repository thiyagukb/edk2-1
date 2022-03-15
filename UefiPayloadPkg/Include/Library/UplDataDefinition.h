/** @file
  Universal Payload data definition used by SetUplDataLib.h and GetUplDataLib.h

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UPL_DATA_DEFINITION_H__
#define __UPL_DATA_DEFINITION_H__

#pragma pack(1)
typedef struct {

  UINT64    Base;
  UINT64    Limit;
  UINT64    Translation;
} CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE;

typedef struct {
  CHAR8                   Identifier[16];
  EFI_PHYSICAL_ADDRESS    Base;
  UINT64                  Size;
} UNIVERSAL_PAYLOAD_EXTRA_DATA_ENTRY_DATA;

typedef struct {
UINT32                                          Segment;
UINT64                                          Supports;
UINT64                                          Attributes;
BOOLEAN                                         DmaAbove4G;
BOOLEAN                                         NoExtendedConfigSpace;
UINT64                                          AllocationAttributes;
CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE Bus;
CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE Io;
CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE Mem;
CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE MemAbove4G;
CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE PMem;
CBOR_UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE PMemAbove4G;
UINT32                                          HID;
UINT32                                          UID;
} UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_INFO;


#pragma pack()
#endif
