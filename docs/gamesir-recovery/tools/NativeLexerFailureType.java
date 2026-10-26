// Original Flex fatal error exits(2); exact-original void/nonreturn contract.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;
public class NativeLexerFailureType extends GhidraScript {
 public void run() throws Exception {
  runScript("NativeTerminationTypes.java");
  Function f=currentProgram.getFunctionManager().getFunctionAt(toAddr(0xbe26cc));
  f.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);f.setNoReturn(true);
  f.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,new ParameterImpl("message",new PointerDataType(CharDataType.dataType),currentProgram));
  println("Original Flex yy_fatal_error atELF0xae26cc calls exit(2); source nonreturn/void contract");
 }
}
