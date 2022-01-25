/** @file
  ELF Load Image Support

Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/LoadFile.h>
#include "Proto.h"
#include "tinycbor\src\cbor.h"
#include <CborHandler.h>
#include <Library/CborWrapperLib.h>



EFI_STATUS
EFIAPI
SaveUint (
  VOID *Key,
  UINTN Value
) 
{
  PEI_CBOR_HANDLER_PPI                *cbor_handler;
  EFI_STATUS                          Status;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  DEBUG((DEBUG_ERROR,"KBT SaveUint cbor_handler->gEncoder : %p \n",cbor_handler->gEncoder));
  cbor_handler->Set_UINT(Key,Value);
  return Status;
}

EFI_STATUS
EFIAPI
SaveInt (
  VOID *Key,
  INTN Value
) 
{
  PEI_CBOR_HANDLER_PPI                *cbor_handler;
  EFI_STATUS                          Status;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  
  cbor_handler->Set_INT(Key,Value);
  return Status;
}

EFI_STATUS
EFIAPI
Save_Text_Strings (
  VOID *Key,
  const char * Value
) 
{
  PEI_CBOR_HANDLER_PPI                *cbor_handler;
  EFI_STATUS                          Status;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  DEBUG((DEBUG_ERROR,"KBT Save_Text_Strings cbor_handler->gEncoder : %p \n",cbor_handler->gEncoder));
  cbor_handler->Set_Text_Strings(Key,Value);
  return Status;
}

EFI_STATUS
EFIAPI
Save_Byte_Strings (
  VOID *Key,
  const uint8_t * Value,
  UINTN Size
) 
{
  PEI_CBOR_HANDLER_PPI                *cbor_handler;
  EFI_STATUS                          Status;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  
  cbor_handler->Set_Byte_Strings(Key,Value,Size);
  return Status;
}

EFI_STATUS
EFIAPI
Save_Negative_INT  (
  VOID *Key,
  INT64 Value
) 
{
  PEI_CBOR_HANDLER_PPI                *cbor_handler;
  EFI_STATUS                          Status;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  
  cbor_handler->Set_Negative_INT(Key,Value);
  return Status;
}

EFI_STATUS
EFIAPI
Save_BOOLEAN (
  VOID *Key,
  BOOLEAN Value
) 
{
  PEI_CBOR_HANDLER_PPI                *cbor_handler;
  EFI_STATUS                          Status;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  
  cbor_handler->Set_BOOLEAN(Key,Value);
  return Status;
}


EFI_STATUS
EFIAPI
Save_Pci_Root_Bridge_Info_Data (
  UNIVERSAL_PAYLOAD_PCI_ROOT_BRIDGE_INFO    *Data,
  UINTN                                     Count
)
{
  PEI_CBOR_HANDLER_PPI   *cbor_handler;
  EFI_STATUS             Status;
 // CborEncoder            *Arraymap;//,SubMap;
  //UINTN                  Index;

  Status = PeiServicesLocatePpi (
            &gPeiCborHandlerPpiGuid,
            0,
            NULL,
            (VOID **)&cbor_handler
            );
  
  Save_Text_Strings(cbor_handler->gEncoder,"RootBridgeInfo.Data");
  /*Arraymap = (CborEncoder *)cbor_handler->Init_Array(cbor_handler->gEncoder,Count);
  cbor_handler->SubMap_Init(Arraymap,&SubMap,52);
  
  for (Index = 0;Index<Count;Index++) {*/
    Save_Text_Strings(cbor_handler->gEncoder,"Segment");
    SaveUint(cbor_handler->gEncoder,Data->Segment);
    Save_Text_Strings(cbor_handler->gEncoder,"Supports");
    SaveUint(cbor_handler->gEncoder,Data->Supports);
    Save_Text_Strings(cbor_handler->gEncoder,"Attributes");
    SaveUint(cbor_handler->gEncoder,Data->Attributes);
    Save_Text_Strings(cbor_handler->gEncoder,"DmaAbove4G");
    SaveUint(cbor_handler->gEncoder,Data->DmaAbove4G);
    Save_Text_Strings(cbor_handler->gEncoder,"NoExtendedConfigSpace");
    SaveUint(cbor_handler->gEncoder,Data->NoExtendedConfigSpace);
    Save_Text_Strings(cbor_handler->gEncoder,"AllocationAttributes");
    SaveUint(cbor_handler->gEncoder,Data->AllocationAttributes);
    Save_Text_Strings(cbor_handler->gEncoder,"Bus.Base");
    SaveUint(cbor_handler->gEncoder,Data->Bus.Base);
    Save_Text_Strings(cbor_handler->gEncoder,"Bus.Limit");
    SaveUint(cbor_handler->gEncoder,Data->Bus.Limit);
    Save_Text_Strings(cbor_handler->gEncoder,"Bus.Translation ");
    SaveUint(cbor_handler->gEncoder,Data->Bus.Translation);
    Save_Text_Strings(cbor_handler->gEncoder,"Io.Base");
    SaveUint(cbor_handler->gEncoder,Data->Io.Base);
    Save_Text_Strings(cbor_handler->gEncoder,"Io.Limit");
    SaveUint(cbor_handler->gEncoder,Data->Io.Limit);
    Save_Text_Strings(cbor_handler->gEncoder,"Io.Translation");
    SaveUint(cbor_handler->gEncoder,Data->Io.Translation);
    Save_Text_Strings(cbor_handler->gEncoder,"Mem.Base");
    SaveUint(cbor_handler->gEncoder,Data->Mem.Base);
    Save_Text_Strings(cbor_handler->gEncoder,"Mem.Limit");
    SaveUint(cbor_handler->gEncoder,Data->Mem.Limit);
    Save_Text_Strings(cbor_handler->gEncoder,"Mem.Translation");
    SaveUint(cbor_handler->gEncoder,Data->Mem.Translation);
    Save_Text_Strings(cbor_handler->gEncoder,"MemAbove4G.Base");
    SaveUint(cbor_handler->gEncoder,Data->MemAbove4G.Base);
    Save_Text_Strings(cbor_handler->gEncoder,"MemAbove4G.Limit");
    SaveUint(cbor_handler->gEncoder,Data->MemAbove4G.Limit);
    Save_Text_Strings(cbor_handler->gEncoder,"MemAbove4G.Translation");
    SaveUint(cbor_handler->gEncoder,Data->MemAbove4G.Translation);
    Save_Text_Strings(cbor_handler->gEncoder,"PMem.Base");
    SaveUint(cbor_handler->gEncoder,Data->PMem.Base);
    Save_Text_Strings(cbor_handler->gEncoder,"PMem.Limit");
    SaveUint(cbor_handler->gEncoder,Data->PMem.Limit);
    Save_Text_Strings(cbor_handler->gEncoder,"PMem.Translation");
    SaveUint(cbor_handler->gEncoder,Data->PMem.Translation);
    Save_Text_Strings(cbor_handler->gEncoder,"PMemAbove4G.Base");
    SaveUint(cbor_handler->gEncoder,Data->PMemAbove4G.Base);
    Save_Text_Strings(cbor_handler->gEncoder,"PMemAbove4G.Limit");
    SaveUint(cbor_handler->gEncoder,Data->PMemAbove4G.Limit);
    Save_Text_Strings(cbor_handler->gEncoder,"PMemAbove4G.Translation");
    SaveUint(cbor_handler->gEncoder,Data->PMemAbove4G.Translation);
    Save_Text_Strings(cbor_handler->gEncoder,"PMemAbove4G.HID");
    SaveUint(cbor_handler->gEncoder,Data->HID);
    Save_Text_Strings(cbor_handler->gEncoder,"PMemAbove4G.UID");
    SaveUint(cbor_handler->gEncoder,Data->UID);
  //}
   DEBUG((DEBUG_ERROR,"KBT Save_Pci_Root_Bridge_Info_Data \n"));
//  cbor_handler->Close_Map(Arraymap,&SubMap);
return EFI_SUCCESS;
}

