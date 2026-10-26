// Source-declared nonreturning SPIR-V diagnostics; analysis-only original ABI.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;
public class NativeVtnFailureTypes extends GhidraScript {
 public void run() throws Exception {
  for(long a:new long[]{0xd84cb0L,0xd84eecL,0xd84fd0L}) {
   Function f=currentProgram.getFunctionManager().getFunctionAt(toAddr(a));
   println("Before "+f.getName()+" "+f.getEntryPoint()+" noreturn="+f.hasNoReturn());
   f.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);
   if(a==0xd84cb0L) {
    f.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
     new ParameterImpl("builder",new PointerDataType(VoidDataType.dataType),currentProgram),
     new ParameterImpl("file",new PointerDataType(CharDataType.dataType),currentProgram),
     new ParameterImpl("line",UnsignedIntegerDataType.dataType,currentProgram),
     new ParameterImpl("format",new PointerDataType(CharDataType.dataType),currentProgram));
    f.setVarArgs(true);
   }
   f.setNoReturn(true);
   println("After "+f.getName()+" noreturn="+f.hasNoReturn());
  }
 }
}
