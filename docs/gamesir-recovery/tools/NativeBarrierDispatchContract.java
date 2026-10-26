// Exact-original finite gen6/7/8 TU barrier tail; read-only analysis only.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;
public class NativeBarrierDispatchContract extends GhidraScript {
 public void run() throws Exception {
  Function fn=currentProgram.getFunctionManager().getFunctionAt(toAddr(0xa59224));
  fn.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);
  fn.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
   new ParameterImpl("cmd",new PointerDataType(VoidDataType.dataType),currentProgram),
   new ParameterImpl("dep_count",UnsignedIntegerDataType.dataType,currentProgram),
   new ParameterImpl("dep_infos",new PointerDataType(VoidDataType.dataType),currentProgram),
   new ParameterImpl("no_sync",BooleanDataType.dataType,currentProgram));
  for(long elf:new long[]{0x959af8L,0x959b54L,0x959bb0L}) {
   Function dst=currentProgram.getFunctionManager().getFunctionAt(toAddr(elf+0x100000));
   dst.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);
   dst.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
    new ParameterImpl("cache",new PointerDataType(VoidDataType.dataType),currentProgram),
    new ParameterImpl("src_stage",UnsignedIntegerDataType.dataType,currentProgram),
    new ParameterImpl("dst_stage",UnsignedIntegerDataType.dataType,currentProgram));
  }
  // ELF0x9597c8..959804: table0xcf6480, index=(chip-6)*8,
  // LDP x29/x30 + ADD SP,#0xc0, BR x3; all3 table values are named entries.
  disassemble(toAddr(0xa59804));
  Instruction tail=getInstructionAt(toAddr(0xa59804));
  if(tail==null||!tail.getMnemonicString().equalsIgnoreCase("br"))throw new Exception("Wrong exact ELF tail");
  tail.setFlowOverride(FlowOverride.CALL_RETURN);
  println("Finite gen6/7/8 barrier dispatch tail: analysis-only call-return and original3-argument callee ABI");
 }
}
