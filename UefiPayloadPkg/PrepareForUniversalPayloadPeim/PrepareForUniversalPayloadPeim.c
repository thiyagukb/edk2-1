/** @file
  Do necessary preparation for Universal Payload

Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/PrepareForUniversalPayload.h>
#include <Library/MemoryMapLib.h>
#include <Library/SetUplDataLib.h>
#include <Uefi/UefiSpec.h>
#include <UniversalPayload/MemoryMap.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <UniversalPayload/SmbiosTable.h>
#include <UniversalPayload/AcpiTable.h>
#include <UniversalPayload/UniversalPayload.h>
#include <UniversalPayload/ExtraData.h>
#include <UniversalPayload/SerialPortInfo.h>
#include <Library/MemoryAllocationLib.h>

RETURN_STATUS
EFIAPI
SetUplDataBasedHob (
  VOID
  )
{
  UNIVERSAL_PAYLOAD_MEMORY_MAP            *MemoryMapHob;
  UINT8                                   *GuidHob;
  RETURN_STATUS                           Status;
  UNIVERSAL_PAYLOAD_ACPI_TABLE            *AcpiTable;
  EFI_HOB_CPU                             *CpuHob;
  EFI_PEI_HOB_POINTERS                    HobBuffer;
  UNIVERSAL_PAYLOAD_RESOURCE_DESCRIPTOR   *ResourceDescriptorBuffer;
  UINTN                                   Count;
  

  GuidHob      = GetFirstGuidHob (&gUniversalPayloadMemoryMapGuid);
  MemoryMapHob = (UNIVERSAL_PAYLOAD_MEMORY_MAP *)GET_GUID_HOB_DATA (GuidHob);
  Status       = SetUplMemoryMap (MemoryMapHob->MemoryMap, MemoryMapHob->Count);

  GuidHob   = GetFirstGuidHob (&gUniversalPayloadAcpiTableGuid);
  AcpiTable = (UNIVERSAL_PAYLOAD_ACPI_TABLE *)GET_GUID_HOB_DATA (GuidHob);
  SetUplUint64 ("AcpiTableRsdp", (UINT64)AcpiTable->Rsdp);

  CpuHob = GetFirstHob (EFI_HOB_TYPE_CPU);
  SetUplUint8 ("MemorySpace", CpuHob->SizeOfMemorySpace);
  SetUplUint8 ("IoSpace", CpuHob->SizeOfIoSpace);
  
  HobBuffer.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
  
  Count = 0;
  while (!END_OF_HOB_LIST (HobBuffer)) {
    if (HobBuffer.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      Count++;
    }
    HobBuffer.Raw = GET_NEXT_HOB (HobBuffer);
  }

  ResourceDescriptorBuffer = AllocatePages(EFI_SIZE_TO_PAGES(sizeof(UNIVERSAL_PAYLOAD_RESOURCE_DESCRIPTOR)*Count));
  HobBuffer.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
  
  Count = 0;
  while (!END_OF_HOB_LIST (HobBuffer)) {
    if (HobBuffer.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
        CopyGuid(&ResourceDescriptorBuffer[Count].Owner,&HobBuffer.ResourceDescriptor->Owner);
        ResourceDescriptorBuffer[Count].ResourceType = HobBuffer.ResourceDescriptor->ResourceType;
        ResourceDescriptorBuffer[Count].ResourceAttribute = HobBuffer.ResourceDescriptor->ResourceAttribute;
        ResourceDescriptorBuffer[Count].PhysicalStart = HobBuffer.ResourceDescriptor->PhysicalStart;
        ResourceDescriptorBuffer[Count].ResourceLength = HobBuffer.ResourceDescriptor->ResourceLength;
      Count++;
    }
    HobBuffer.Raw = GET_NEXT_HOB (HobBuffer);
  }
  SetUplResourceData(ResourceDescriptorBuffer,Count);

  return Status;
}

VOID *
EFIAPI
PrepareForUniversalPayload (
  VOID  *HobStart
  )
{
  RETURN_STATUS         Status;
  VOID                  *Buffer;
  UINTN                 Size;
  EFI_PEI_HOB_POINTERS  Hob;

  DEBUG ((DEBUG_INFO, "Begin to do necessary preparation for Universal Payload\n"));
  Status = BuildMemoryMap ();
  if (RETURN_ERROR (Status)) {
    return NULL;
  }

  SetUplDataBasedHob ();

  Hob.Raw = (VOID *)GetHobList ();
  SetUplUint64 ("HobList", (UINT64)(UINTN)Hob.Raw);

  //
  // Clean non-related Hobs to prove the ability of new UPL interface
  //
  while (!END_OF_HOB_LIST (Hob)) {
    Hob.Raw = GET_NEXT_HOB (Hob);
    if ((Hob.Header->HobType == EFI_HOB_TYPE_HANDOFF) || (Hob.Header->HobType == EFI_HOB_TYPE_END_OF_HOB_LIST)) {
      continue;
    }

    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      if (CompareGuid (&Hob.Guid->Name, &gPcdDataBaseHobGuid)) {
        continue;
      }
    }

    Hob.Header->HobType = EFI_HOB_TYPE_UNUSED;
  }

  LockUplAndGetBuffer (&Buffer, &Size);

  return Buffer;
}

EDKII_PREPARE_FOR_UNIVERSAL_PAYLOAD_PPI  gPrepareForUniveralPayloadPpi = {
  PrepareForUniversalPayload
};

EFI_PEI_PPI_DESCRIPTOR  gPrepareForUniversalPayloadPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEdkiiPrepareForUniversalPayloadPpiGuid,
  &gPrepareForUniveralPayloadPpi
};

/**

  Install Prepare For Universal Payload PPI.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The entry point executes successfully.
  @retval Others      Some error occurs during the execution of this function.

**/
EFI_STATUS
EFIAPI
InitializePrepareForUniversalPayload (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesInstallPpi (&gPrepareForUniversalPayloadPpiList);

  return Status;
}
