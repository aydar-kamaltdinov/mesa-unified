// Apply only recovered original API/stack types. No bytes/flow/engine changes.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
import java.util.*;
public class NativeFeatureCheckerTypes extends GhidraScript {
 public void run() throws Exception {
  Function fn=currentProgram.getFunctionManager().getFunctionAt(toAddr(0xb24460));
  Map<String,StructureDataType> types=new HashMap<>();
  List<String> rows=Files.readAllLines(Paths.get(getScriptArgs()[0]));
  for(String row:rows) {
   String[] x=row.split("\t",4);int size=Integer.parseInt(x[1]);StructureDataType t=new StructureDataType("native_"+x[2],size);
   t.replaceAtOffset(0,UnsignedIntegerDataType.dataType,4,"sType",null);
   t.replaceAtOffset(8,new PointerDataType(VoidDataType.dataType),8,"pNext",null);
   for(String field:x[3].split(";")) {String[] y=field.split(":",2);t.replaceAtOffset(Integer.parseInt(y[0]),UnsignedIntegerDataType.dataType,4,y[1],"Original field route");}
   types.put(x[2],t);
  }
  StructureDataType root=types.get("VkPhysicalDeviceFeatures2");
  StructureDataType core=new StructureDataType("native_VkPhysicalDeviceFeatures",220);
  for(DataTypeComponent c:root.getDefinedComponents())if(c.getOffset()>=16)core.replaceAtOffset(c.getOffset()-16,c.getDataType(),4,c.getFieldName(),null);
  StructureDataType physical=new StructureDataType("native_checker_physical_prefix",0x1490);
  physical.replaceAtOffset(0xc,BooleanDataType.dataType,1,"features_queried_latch",null);
  StructureDataType ext=new StructureDataType("native_checker_extension_bytes",0);
  for(String row:Files.readAllLines(Paths.get(getScriptArgs()[1]))) {String[] x=row.split("\t",2);if(ext.getNumComponents()!=Integer.parseInt(x[0]))throw new Exception("Extension index mismatch");ext.add(BooleanDataType.dataType,x[1],null);}
  physical.replaceAtOffset(0x58,ext,431,"supported_extensions",null);
  physical.replaceAtOffset(0x448,UnsignedIntegerDataType.dataType,4,"apiVersion",null);
  FunctionDefinitionDataType query=new FunctionDefinitionDataType("native_checker_GetPhysicalDeviceFeatures2");query.setReturnType(VoidDataType.dataType);
  query.setArguments(new ParameterDefinition[]{new ParameterDefinitionImpl("physical",new PointerDataType(physical),null),new ParameterDefinitionImpl("features",new PointerDataType(root),null)});
  physical.replaceAtOffset(0x1488,new PointerDataType(query),8,"GetPhysicalDeviceFeatures2",null);
  StructureDataType create=new StructureDataType("native_VkDeviceCreateInfo_prefix",72);
  create.replaceAtOffset(0,UnsignedIntegerDataType.dataType,4,"sType",null);create.replaceAtOffset(8,new PointerDataType(VoidDataType.dataType),8,"pNext",null);create.replaceAtOffset(64,new PointerDataType(core),8,"pEnabledFeatures",null);
  for(Variable v:fn.getLocalVariables())fn.removeVariable(v);
  fn.setReturnType(IntegerDataType.dataType,SourceType.USER_DEFINED);
  fn.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,new ParameterImpl("physical",new PointerDataType(physical),currentProgram),new ParameterImpl("createInfo",new PointerDataType(create),currentProgram));
  for(String row:rows) {String[] x=row.split("\t",4);fn.addLocalVariable(new LocalVariableImpl("supported_"+x[2],types.get(x[2]),Integer.parseInt(x[0]),currentProgram),SourceType.USER_DEFINED);}
  Function helper=currentProgram.getFunctionManager().getFunctionAt(toAddr(0xb3b62c));helper.setReturnType(IntegerDataType.dataType,SourceType.USER_DEFINED);
  helper.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,new ParameterImpl("physical",new PointerDataType(physical),currentProgram),new ParameterImpl("supported",new PointerDataType(core),currentProgram),new ParameterImpl("enabled",new PointerDataType(core),currentProgram),new ParameterImpl("struct_name",new PointerDataType(CharDataType.dataType),currentProgram));
  println("Applied275disjoint recovered original stack structures, API pointers and exact void query prototype; no flow override");
 }
}
