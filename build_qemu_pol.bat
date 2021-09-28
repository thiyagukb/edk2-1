@setlocal

@if "%1" == "entry" (
  goto entry
)
@if "%1" == "objcopy" (
  goto objcopy
)

@if "%1" == "ovmf" (
  goto ovmf
)

call build -p UefiPayloadPkg\UefiPayloadPkg.dsc -a X64 -D UNIVERSAL_PAYLOAD=TRUE -t CLANGPDB -y upllog.txt
if not %ERRORLEVEL% == 0 exit /b 1

:entry
call build -p UefiPayloadPkg\UefiPayloadPkg.dsc -a X64 -D UNIVERSAL_PAYLOAD=TRUE -m UefiPayloadPkg\UefiPayloadEntry\UniversalPayloadEntry.inf -t CLANGDWARF 
if not %ERRORLEVEL% == 0 exit /b 1


:objcopy
@set FV=Build\UefiPayloadPkgX64\DEBUG_CLANGPDB\FV\DXEFV.Fv
@set ENTRY=Build\UefiPayloadPkgX64\DEBUG_CLANGDWARF\X64\UefiPayloadPkg\UefiPayloadEntry\UniversalPayloadEntry\DEBUG\UniversalPayloadEntry.dll

@REM 
@REM If dependent sources are not changed, entry.elf isn't updated by build tool.
@REM So, if we don't remove the uefi_fv section before adding, there will be 2 uefi_fv sections.
@REM
@REM Remove .upld.uefi_fv section because entry.elf isn't updated if dependent sources are not changed.
"C:\Program Files\LLVM\bin\llvm-objcopy" -I elf64-x86-64 -O elf64-x86-64 --remove-section .upld.uefi_fv %ENTRY%
@if not %ERRORLEVEL% == 0 exit /b 1
"C:\Program Files\LLVM\bin\llvm-objcopy" -I elf64-x86-64 -O elf64-x86-64 --add-section .upld.uefi_fv=%FV% %ENTRY%
@if not %ERRORLEVEL% == 0 exit /b 1
"C:\Program Files\LLVM\bin\llvm-objcopy" -I elf64-x86-64 -O elf64-x86-64 --set-section-alignment .upld.uefi_fv=16 %ENTRY%
@if not %ERRORLEVEL% == 0 exit /b 1

:ovmf
call build -p OvmfPkg\QemuUniversalPayload\OvmfPkgPol.dsc -a IA32 -a X64 -D DEBUG_ON_SERIAL_PORT -t CLANGPDB -y ovmflog.txt

@if not %ERRORLEVEL% == 0 exit /b 1
call C:\code\Payload\edk2_nanopb2\qemu\qemu-system-x86_64.exe -machine q35 -drive file=C:\code\Payload\edk2_tinycbor\edk2-for_upstream_test\Build\OvmfPol\DEBUG_CLANGPDB\FV\OVMF.fd,if=pflash,format=raw -boot menu=on,splash-time=0 -usb -device nec-usb-xhci,id=xhci -device usb-kbd -device usb-mouse -net none -serial mon:stdio