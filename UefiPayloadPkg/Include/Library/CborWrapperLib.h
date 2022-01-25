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

#ifndef __CBOR_WRAPPER_LIB__
#define __CBOR_WRAPPER_LIB__

#include <UniversalPayload/PciRootBridges.h>

typedef struct {
UINT32              Segment;               
UINT64              Supports;                                                                                                                                                      
UINT64              Attributes;                                                                                                                                                    
BOOLEAN          DmaAbove4G;            
BOOLEAN          NoExtendedConfigSpace;           UINT64              AllocationAttributes;  
UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE Bus;                   
UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE Io;                    
UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE Mem;                   
UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE MemAbove4G;            
UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE PMem;                  
UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_APERTURE PMemAbove4G;           
UINT32                                     HID;                                                                         
UINT32                                     UID;                                                           
} UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_INFO;


EFI_STATUS
EFIAPI
SaveUint (
  VOID *Key,
  UINTN Value
);

EFI_STATUS
EFIAPI
SaveInt (
  VOID *Key,
  INTN Value
);

EFI_STATUS
EFIAPI
Save_Text_Strings (
  VOID *Key,
  const char * Value
);

EFI_STATUS
EFIAPI
Save_Byte_Strings (
  VOID *Key,
  const UINT8 * Value,
  UINTN Size
) ;

EFI_STATUS
EFIAPI
Save_Negative_INT  (
  VOID *Key,
  INT64 Value
);

EFI_STATUS
EFIAPI
Save_BOOLEAN (
  VOID *Key,
  BOOLEAN Value
);

EFI_STATUS
EFIAPI
Save_Pci_Root_Bridge_Info_Data (
      UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_INFO    *Data,
      UINTN                                     Count
);

#endif

