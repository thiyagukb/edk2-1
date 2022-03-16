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

RETURN_STATUS
EFIAPI
SetCbor (
  OUT VOID                **Buffer,
  OUT UINTN               *Size
  );

VOID *
EFIAPI
PrepareForUniversalPayload (
  VOID  *HobStart
  )
{
  RETURN_STATUS                 Status;
  VOID                          *MemHob;
  UNIVERSAL_PAYLOAD_MEMORY_MAP  *MemoryMapHob;
  EFI_PEI_HOB_POINTERS                      Hob;


  DEBUG ((DEBUG_INFO, "Begin to do necessary preparation for Universal Payload\n"));
  Status = BuildMemoryMap ();
  if (RETURN_ERROR (Status)) {
    return NULL;
  }

  MemHob          = GetFirstGuidHob (&gUniversalPayloadMemoryMapGuid);
  MemoryMapHob = (UNIVERSAL_PAYLOAD_MEMORY_MAP *)GET_GUID_HOB_DATA (MemHob);
  DEBUG ((DEBUG_INFO, "SetUplMemoryMap\n"));
  SetUplMemoryMap (MemoryMapHob->MemoryMap, MemoryMapHob->Count);
  Hob.Raw = (VOID *)GetHobList ();
  SetUplUint64 ("HobList", (UINT64)(UINTN)Hob.Raw);

  VOID   *Data;
  VOID   *Buffer;
  UINTN  Size;
  //SetCbor (&Buffer, &Size);
  LockUplAndGetBuffer (&Buffer, &Size);
  Data = BuildGuidHob (&gCborBufferGuid, Size);
  CopyMem(Data, Buffer, Size);
DEBUG ((DEBUG_INFO, "HobStart = 0x%x\n", (UINT32) HobStart));

  /*
  Hob.Raw = (VOID *)GetHobList ();
  while (!END_OF_HOB_LIST (Hob)) {
    Hob.Raw = GET_NEXT_HOB (Hob);
    if (Hob.Header->HobType == EFI_HOB_TYPE_HANDOFF || Hob.Header->HobType == EFI_HOB_TYPE_END_OF_HOB_LIST || Hob.Header->HobType ==  EFI_HOB_TYPE_CPU ||
        Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR || Hob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION) {
      continue;
    }
    //if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
    //  if (CompareGuid (&Hob.Guid->Name, &gUniversalPayloadAcpiTableGuid) || CompareGuid (&Hob.Guid->Name, &gUniversalPayloadSerialPortInfoGuid) ||
    //      CompareGuid (&Hob.Guid->Name, &gUniversalPayloadSmbios3TableGuid) || CompareGuid (&Hob.Guid->Name, &gUniversalPayloadSmbiosTableGuid) ||
    //      CompareGuid (&Hob.Guid->Name, &gUefiAcpiBoardInfoGuid) || CompareGuid (&Hob.Guid->Name, &gUniversalPayloadPciRootBridgeInfoGuid) ||
    //      CompareGuid (&Hob.Guid->Name, &gUniversalPayloadExtraDataGuid) || CompareGuid (&Hob.Guid->Name, &gPcdDataBaseHobGuid) ||
    //      CompareGuid (&Hob.Guid->Name, &gCborBufferGuid)) {
    //    continue;
    //  }
    //}
    Hob.Header->HobType = EFI_HOB_TYPE_UNUSED;
  }
  */

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
