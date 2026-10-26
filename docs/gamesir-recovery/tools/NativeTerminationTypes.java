// Exact-original C++ runtime failure contracts, read-only analysis only.
// These names denote nonreturning diagnostics/throws, not GPU terminate ops.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
public class NativeTerminationTypes extends GhidraScript {
 public void run() throws Exception {
  long[] elf={0x8ed3dcL,0x8ed3f4L,0x8ef790L,0x8f00c0L,0x8f0868L,
   0xcc33b4L,0xcc3ae8L,0xcc3d84L,0xcc3ee4L,0xcc4218L,0xcc426cL,
   0xcc4800L,0xcc4d94L,0xcc4db4L,0xcc5474L,0xcc54c8L,0xcdfe14L,0xce0734L};
  for(long a:elf) {
   Function f=currentProgram.getFunctionManager().getFunctionAt(toAddr(a+0x100000));
   f.setReturnType(VoidDataType.dataType,ghidra.program.model.symbol.SourceType.USER_DEFINED);
   f.setNoReturn(true);println("Source runtime nonreturn contract "+f.getName()+" @ "+f.getEntryPoint());
  }
  // Preserve the separately recovered exact SPIR-V diagnostics as well.
  runScript("NativeVtnFailureTypes.java");
 }
}
