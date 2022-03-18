/** @file
  Universal Payload data definition used by SetUplDataLib.h and GetUplDataLib.h

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UPL_DATA_DEFINITION_H__
#define __UPL_DATA_DEFINITION_H__

#include <Uefi/UefiSpec.h>
#include <UniversalPayload/PciRootBridges.h>
#include <UniversalPayload/ExtraData.h>

#define ACPI_TABLE_RSDP                 "AcpiTableRsdp"
#define MEMORY_SPACE                    "MemorySpace"
#define IO_SPACE                        "IoSpace"
#define MEMORY_MAP                      "MemoryMap"
#define HOB_LIST                        "HobList"
#define BASE                            "Base"
#define NUMBER_OF_PAGES                 "NumberOfPages"
#define TYPE                            "Type"
#define ATTRIBUTE                       "Attribute"
#define SERIAL_PORT_BAUDRATE            "SerialPortBaudRate"
#define SERIAL_PORT_USE_MMIO            "SerialPortUseMmio"
#define SERIAL_PORT_REGISTER_BASE       "SerialPortRegisterBase"
#define SERIAL_PORT_REGISTER_STRIDE     "SerialPortRegisterStride"
#define ROOT_BRIDGE_RESOURCE_ASSIGNED   "RootBridgeResourceAssigned"
#define UPL_EXTRA_DATA                  "UplExtradata"
#define IDENTIFIER                      "Identifier"
#define SIZE                            "Size"
#define ROOT_BRIDGE_INFO                "RootBridgeInfo"
#define SEGMENT                         "Segment"
#define SUPPORTS                        "Supports"
#define ATTRIBUTES                      "Attributes"
#define DMA_ABOVE_4G                    "DmaAbove4G"
#define NO_EXTENDED_CONFIG_SPACE        "NoExtendedConfigSpace"
#define ALLOCATION_ATTRIBUTES           "AllocationAttributes"
#define BUS_BASE                        "BusBase"
#define BUS_LIMIT                       "BusLimit"
#define BUS_TRANSLATION                 "BusTranslation"
#define IO_BASE                         "IoBase"
#define IO_LIMIT                        "IoLimit"
#define IO_TRANSLATION                  "IoTranslation"
#define MEM_BASE                        "MemBase"
#define MEM_LIMIT                       "MemLimit"
#define MEM_TRANSLATION                 "MemTranslation"
#define MEM_ABOVE_4G_BASE               "MemAbove4GBase"
#define MEM_ABOVE_4G_LIMIT              "MemAbove4GLimit"
#define MEM_ABOVE_4G_TRANSLATION        "MemAbove4GTranslation"
#define PMEM_BASE                       "PMemBase"
#define PMEM_LIMIT                      "PMemLimit"
#define PMEM_TRANSLATION                "PMemTranslation"
#define PMEM_ABOVE_4G_BASE              "PMemAbove4GBase"
#define PMEM_ABOVE_4G_LIMIT             "PMemAbove4GLimit"
#define PMEM_ABOVE_4G_TRANSLATION       "PMemAbove4GTranslation"
#define ROOT_BRIDGE_HID                             "HID"
#define ROOT_BRIDGE_UID                             "UID"
#define RESOURCE                        "Resource"
#define OWNER                           "Owner"
#define LENGTH                          "Length"
#define RESOURCEALLOCATION              "ResourceAllocation"
#define NAME                            "Name"
#define BUFFER_SIZE                     "BufferSize"

typedef struct {
  EFI_GUID                    Owner;
  EFI_RESOURCE_TYPE           ResourceType;
  EFI_RESOURCE_ATTRIBUTE_TYPE ResourceAttribute;
  EFI_PHYSICAL_ADDRESS        PhysicalStart;
  UINT64                      ResourceLength;
} UNIVERSAL_PAYLOAD_RESOURCE_DESCRIPTOR;

typedef struct {
  EFI_GUID                Name;
  EFI_PHYSICAL_ADDRESS    MemoryBaseAddress;
  UINT64                  MemoryLength;
  EFI_MEMORY_TYPE         MemoryType;
} UNIVERSAL_PAYLOAD_MEMORY_ALLOCATION;

#endif
