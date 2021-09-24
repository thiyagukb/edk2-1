

#include "tinycbor\src\cbor.h"


#include <Base.h>
#include <Library/DebugLib.h>
#include "Proto.h"
#include <Library/MemoryAllocationLib.h>
#define ROW_LIMITER 16


UINT8 buf11[16];

UINTN
PrintHex1 (
  IN  UINT8         *DataStart,
  IN  UINT16         DataSize
  )
{
  UINTN  Index1;
  UINTN  Index2;
  UINT8  *StartAddr;

  StartAddr = DataStart;
  for (Index1 = 0; Index1 * ROW_LIMITER < DataSize; Index1++) {
    DEBUG ((DEBUG_ERROR, "   0x%04p:", (DataStart - StartAddr)));
    for (Index2 = 0; (Index2 < ROW_LIMITER) && (Index1 * ROW_LIMITER + Index2 < DataSize); Index2++){
      DEBUG ((DEBUG_ERROR, " %02x", *DataStart));
      DataStart++;
    }
    DEBUG ((DEBUG_ERROR, "\n"));
  }

  return 0;
}



RETURN_STATUS
EFIAPI
SetCbor (
  OUT VOID                **Buffer,
  OUT UINTN               *Size
  )
{

  
  UINTN   size;
  size = 100;
  UINT8   *buf = AllocatePool(size);

  UINT32 buffer1[] = { 0x12345678, 0x0, 0x90ABCDEF};
  CborEncoder encoder, mapEncoder;
  cbor_encoder_init(&encoder, buf, size, 0);
  cbor_encoder_create_map(&encoder, &mapEncoder, 2);
  cbor_encode_text_stringz(&mapEncoder, "foo");
  cbor_encode_uint(&mapEncoder, 56);
  cbor_encode_text_stringz(&mapEncoder, "baa");
  cbor_encode_byte_string(&mapEncoder, (const uint8_t *)buffer1, 12);
  cbor_encoder_close_container(&encoder, &mapEncoder);

  
  UINTN used_size;
  used_size = cbor_encoder_get_buffer_size (&encoder, (const uint8_t *)buf);
  DEBUG ((EFI_D_ERROR, "used_size: 0x%x \n", used_size ));
  PrintHex1(buf,used_size);

  CborParser parser;
  CborValue value;
  DEBUG ((DEBUG_ERROR, "kuang %a: %d\n", __FILE__, __LINE__)); 
  cbor_parser_init(buf, used_size, 0, &parser, &value);
  DEBUG ((DEBUG_ERROR, "kuang %a: %d\n", __FILE__, __LINE__)); 
  CborValue element;
    DEBUG ((EFI_D_ERROR, "value ptr: %p \n", value.source.ptr ));
  DEBUG ((EFI_D_ERROR, "value remaining: %d \n", value.remaining ));
  DEBUG ((EFI_D_ERROR, "value type: 0x%x \n", value.type ));
  DEBUG ((EFI_D_ERROR, "value extra: 0x%x \n", value.extra ));
  DEBUG ((EFI_D_ERROR, "value flags: 0x%x \n", value.flags ));


  int a = cbor_value_map_find_value (&value,  "baa", &element);

  
  DEBUG ((EFI_D_ERROR, "a: 0x%x \n", a ));
  DEBUG ((EFI_D_ERROR, "element ptr: %p \n", element.source.ptr ));
  DEBUG ((EFI_D_ERROR, "element remaining: %d \n", element.remaining ));
  DEBUG ((EFI_D_ERROR, "element type: 0x%x \n", element.type ));
  DEBUG ((EFI_D_ERROR, "element extra: 0x%x \n", element.extra ));
  DEBUG ((EFI_D_ERROR, "element flags: 0x%x \n", element.flags ));
  PrintHex1((UINT8         *)(element.source.ptr),(UINT16)(element.remaining));
  UINT8   buf3[30];
  UINTN   size3;
  size3 = 30;
  cbor_value_copy_byte_string (&element,  buf3, &size3, NULL);
  PrintHex1(buf3,30);

  cbor_value_map_find_value (&value, "foo", &element);

    DEBUG ((EFI_D_ERROR, "value ptr: %p \n", value.source.ptr ));
  DEBUG ((EFI_D_ERROR, "value remaining: %d \n", value.remaining ));
  DEBUG ((EFI_D_ERROR, "value type: 0x%x \n", value.type ));
  DEBUG ((EFI_D_ERROR, "value extra: 0x%x \n", value.extra ));
  DEBUG ((EFI_D_ERROR, "value flags: 0x%x \n", value.flags ));
  DEBUG ((EFI_D_ERROR, "element ptr: %p \n", element.source.ptr ));
  DEBUG ((EFI_D_ERROR, "element remaining: %d \n", element.remaining ));
  DEBUG ((EFI_D_ERROR, "element type: 0x%x \n", element.type ));
  DEBUG ((EFI_D_ERROR, "element extra: 0x%x \n", element.extra ));
  DEBUG ((EFI_D_ERROR, "element flags: 0x%x \n", element.flags ));
  DEBUG ((DEBUG_ERROR, "kuang %a: %d\n", __FILE__, __LINE__)); 
  UINT64 result;
  DEBUG ((DEBUG_ERROR, "kuang %a: %d\n", __FILE__, __LINE__)); 
  //cbor_value_advance(&value);
  DEBUG ((DEBUG_ERROR, "kuang %a: %d\n", __FILE__, __LINE__)); 
  cbor_value_get_uint64(&element, &result);
  DEBUG ((DEBUG_ERROR, "kuang %a: %d\n", __FILE__, __LINE__)); 
  DEBUG ((EFI_D_ERROR, "result: %d \n", result ));


  PrintHex1((UINT8         *)(value.source.ptr),(UINT16)(value.remaining));

   // UINT8* buffer = AllocatePool(128);
   // UINTN message_length;
   // BOOLEAN status;
   // UsefulBufC buf;
   // UINT32 buffer1[] = { 0x12345678, 0x0, 0x90ABCDEF};
   // buf.ptr = buffer1;
   // buf.len = ARRAY_SIZE(buffer1) * 4;
//
//
   // MY_PROTO_BUFFER                          MyProtoBuffer  = MY_PROTO_BUFFER_init_zero;
//
//
   // pb_ostream_t stream = pb_ostream_from_buffer(buffer, 128);
   // 
   // MyProtoBuffer.has_SerialPortInfo = TRUE;
   // MyProtoBuffer.SerialPortInfo.BaudRate = PcdGet32 (PcdSerialBaudRate);
   // MyProtoBuffer.SerialPortInfo.RegisterBase = PcdGet64 (PcdSerialRegisterBase);
   // MyProtoBuffer.SerialPortInfo.RegisterStride = (UINT8) PcdGet32 (PcdSerialRegisterStride);
   // MyProtoBuffer.SerialPortInfo.UseMmio = PcdGetBool (PcdSerialUseMmio);
   // MyProtoBuffer.RawBytes.funcs.encode = &write_string; 
   // MyProtoBuffer.RawBytes.arg = (VOID*)&buf; 
   // 
//
   // MyProtoBuffer.has_MyGuid = TRUE;
   // CopyMem(MyProtoBuffer.MyGuid, &gUefiPayloadPkgTokenSpaceGuid, 16);
//
   // 
//
   // /* Now we are ready to encode the message! */
   // status = pb_encode(&stream, MY_PROTO_BUFFER_fields, &MyProtoBuffer);
   // message_length = (UINTN)stream.bytes_written;
   // *Buffer = buffer;
   // *Size = message_length;
   // DEBUG ((DEBUG_ERROR, "%a: Size = %d\n status =0x%x\n",  __FUNCTION__, *Size, status));
   // PrintHex1(*Buffer,(UINT16)*Size );

  return 0;
}
