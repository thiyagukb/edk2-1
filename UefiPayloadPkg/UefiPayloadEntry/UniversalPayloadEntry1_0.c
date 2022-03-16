/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiPayloadEntry.h"
#include <Library/GetUplDataLib.h>

extern VOID  *mHobList;

/**
  Print all HOBs info from the HOB list.

  @return The pointer to the HOB list.
**/
VOID
PrintHob (
  IN CONST VOID  *HobStart
  );

EFI_STATUS
InitHobList (
  )
{
  UINTN                  MemoryMapCount;
  UINTN                  MemoryMapNumber;
  EFI_MEMORY_DESCRIPTOR  MemoryMap;
  UINTN                  FindIndex;
  UINTN                  Index;
  EFI_PHYSICAL_ADDRESS   FreeMemoryBottom;
  EFI_PHYSICAL_ADDRESS   FreeMemoryTop;
  EFI_PHYSICAL_ADDRESS   MemoryBottom;
  EFI_PHYSICAL_ADDRESS   MemoryTop;

  MemoryMapCount = 0;

  GetUplMemoryMap (&MemoryMap, &MemoryMapCount, 0);
  FindIndex = MemoryMapCount;
  DEBUG ((DEBUG_INFO, "MemoryMapCount%x\n", MemoryMapCount));
  for (Index = 0; Index < MemoryMapCount; Index++) {
    DEBUG ((DEBUG_INFO, "Index%x\n", Index));
    MemoryMapNumber = 1;
    GetUplMemoryMap (&MemoryMap, &MemoryMapNumber, Index);
    DEBUG ((DEBUG_INFO, "MemoryMap.PhysicalStart %lx\n", MemoryMap.PhysicalStart));
    DEBUG ((DEBUG_INFO, "MemoryMap.NumberOfPages %lx %lx\n", MemoryMap.NumberOfPages, FixedPcdGet32 (PcdSystemMemoryUefiRegionSize)));
    DEBUG ((DEBUG_INFO, "MemoryMap.Type %lx\n", MemoryMap.Type));
    //
    // Skip above 4G memory
    //
    if (MemoryMap.PhysicalStart + EFI_PAGES_TO_SIZE (MemoryMap.NumberOfPages) > BASE_4GB) {
      continue;
    }

    if ((MemoryMap.Type != EfiConventionalMemory) || (EFI_PAGES_TO_SIZE (MemoryMap.NumberOfPages) < FixedPcdGet32 (PcdSystemMemoryUefiRegionSize))) {
      continue;
    }

    FindIndex = Index;
  }

  DEBUG ((DEBUG_INFO, "FindIndex%x\n", FindIndex));
  if (FindIndex == MemoryMapCount) {
    return EFI_NOT_FOUND;
  }

  MemoryMapNumber = 1;
  GetUplMemoryMap (&MemoryMap, &MemoryMapNumber, FindIndex);

  MemoryTop        = EFI_PAGES_TO_SIZE (MemoryMap.NumberOfPages) + MemoryMap.PhysicalStart;
  FreeMemoryTop    = MemoryTop;
  MemoryBottom     = MemoryTop - FixedPcdGet32 (PcdSystemMemoryUefiRegionSize);
  FreeMemoryBottom = MemoryBottom;
  DEBUG ((DEBUG_INFO, "MemoryTop%lx\n", MemoryTop));
  HobConstructor ((VOID *)(UINTN)MemoryBottom, (VOID *)(UINTN)MemoryTop, (VOID *)(UINTN)FreeMemoryBottom, (VOID *)(UINTN)FreeMemoryTop);

  UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO  *Serial;

  Serial = BuildGuidHob (&gUniversalPayloadSerialPortInfoGuid, sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO));

  UINT64  TempData;

  GetUplUint64 ("SerialPortBaudRate", &TempData);

  Serial->BaudRate = (UINT32)TempData;
  GetUplUint64 ("SerialPortRegisterBase", &TempData);
  Serial->RegisterBase = (UINT64)TempData;
  GetUplUint64 ("SerialPortRegisterStride", &TempData);
  Serial->RegisterStride  = (UINT8)TempData;
  Serial->Header.Revision = UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO_REVISION;
  Serial->Header.Length   = sizeof (UNIVERSAL_PAYLOAD_SERIAL_PORT_INFO);
  GetUplUint64 ("SerialPortUseMmio", &TempData);
  Serial->UseMmio = (BOOLEAN)TempData;

  return EFI_SUCCESS;
}

BOOLEAN
IsHobNeed (
  EFI_PEI_HOB_POINTERS  Hob
  );

VOID
AddNewHob (
  IN EFI_PEI_HOB_POINTERS  *Hob
  );

RETURN_STATUS
CreateHobsBasedOnMemoryMap1_0 (
  );


EFI_STATUS
EFIAPI
Spec1_0Entry (
  IN UINTN                        BootloaderParameter,
  OUT EFI_FIRMWARE_VOLUME_HEADER  **DxeFv
  )
{
  EFI_STATUS  Status;

  Status = InitUplFromBuffer ((UINTN *)BootloaderParameter, SIZE_64MB);
  DEBUG ((DEBUG_INFO, "Status%x\n", Status));
  InitHobList ();
  // Call constructor for all libraries
  ProcessLibraryConstructorList ();
  DEBUG ((DEBUG_INFO, "Entering Universal Payload...\n"));
  DEBUG ((DEBUG_INFO, "sizeof(UINTN) = 0x%x\n", sizeof (UINTN)));
  InitializeFloatingPointUnits ();
  CreateHobsBasedOnMemoryMap1_0();
  UINT64  TempData;

  Status = GetUplUint64 ("HobList", &TempData);
  DEBUG ((DEBUG_INFO, "Status %lx\n", Status));

  EFI_PEI_HOB_POINTERS  Hob;

  Hob.Raw = (VOID *)(UINTN)TempData;
  //
  // Create an empty FvHob for the DXE FV that contains DXE core.
  //
  BuildFvHob ((EFI_PHYSICAL_ADDRESS)0, 0);
  //
  // Since payload created new Hob, move all hobs except PHIT from boot loader hob list.
  //
  while (!END_OF_HOB_LIST (Hob)) {
    if (IsHobNeed (Hob)) {
      // Add this hob to payload HOB
      AddNewHob (&Hob);
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  UNIVERSAL_PAYLOAD_EXTRA_DATA  *ExtraData;
  UINT8                         *GuidHob;
  EFI_HOB_FIRMWARE_VOLUME       *FvHob;
  UNIVERSAL_PAYLOAD_ACPI_TABLE  *AcpiTable;
  ACPI_BOARD_INFO               *AcpiBoardInfo;

  //
  // Get DXE FV location
  //
  GuidHob = GetFirstGuidHob (&gUniversalPayloadExtraDataGuid);
  ASSERT (GuidHob != NULL);
  ExtraData = (UNIVERSAL_PAYLOAD_EXTRA_DATA *)GET_GUID_HOB_DATA (GuidHob);
  ASSERT (ExtraData->Count == 1);
  ASSERT (AsciiStrCmp (ExtraData->Entry[0].Identifier, "uefi_fv") == 0);

  *DxeFv = (EFI_FIRMWARE_VOLUME_HEADER *)(UINTN)ExtraData->Entry[0].Base;
  ASSERT ((*DxeFv)->FvLength == ExtraData->Entry[0].Size);

  //
  // Create guid hob for acpi board information
  //
  GuidHob = GetFirstGuidHob (&gUniversalPayloadAcpiTableGuid);
  if (GuidHob != NULL) {
    AcpiTable     = (UNIVERSAL_PAYLOAD_ACPI_TABLE *)GET_GUID_HOB_DATA (GuidHob);
    AcpiBoardInfo = BuildHobFromAcpi ((UINT64)AcpiTable->Rsdp);
    ASSERT (AcpiBoardInfo != NULL);
  }

  //
  // Update DXE FV information to first fv hob in the hob list, which
  // is the empty FvHob created before.
  //
  FvHob              = GetFirstHob (EFI_HOB_TYPE_FV);
  FvHob->BaseAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)*DxeFv;
  FvHob->Length      = (*DxeFv)->FvLength;

  return EFI_SUCCESS;
}
