// Exact original partial ABI:431 extension bools at physical+0x58.
// Prefix extent is deliberately not a full tu_physical_device sizeof claim.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
public class NativePhysicalExtensionTypes extends GhidraScript {
 public void run() throws Exception {
  StructureDataType ext=new StructureDataType("native_gs_device_extension_bytes",0);
  for(String row:Files.readAllLines(Paths.get(getScriptArgs()[0]))) {
   String[] x=row.split("\t",2);if(ext.getNumComponents()!=Integer.parseInt(x[0]))throw new Exception("Wrong extension byte index");
   ext.add(BooleanDataType.dataType,x[1],"Original support byte, not Android enumeration result");
  }
  if(ext.getLength()!=431)throw new Exception("Wrong original extension count");
  StructureDataType info=new StructureDataType("native_gs_fd_info_bytes",0x324);
  // Optional named original fields: offsets come from the native debug parser
  // and the original static records, never the current Mesa structure layout.
  if(getScriptArgs().length>1)for(String row:Files.readAllLines(Paths.get(getScriptArgs()[1]))) {
   String[] x=row.split("\t",3);int offset=Integer.parseInt(x[0],16);
   DataType dt=x[1].equals("bool")?BooleanDataType.dataType:x[1].equals("u8")?UnsignedCharDataType.dataType:x[1].equals("f32")?FloatDataType.dataType:UnsignedIntegerDataType.dataType;
   info.replaceAtOffset(offset,dt,dt.getLength(),x[2],"Recovered original field; current Mesa ABI differs");
  }
  StructureDataType p=new StructureDataType("native_gs_physical_extension_prefix",0x58);
  p.add(ext,"supported_extensions",null);
  // Known original pointer field at+0x20b8 bounds the inspected prefix.
  // Pad opaque suffix so Ghidra cannot misprint later offsets as an array
  // of short extension-only structures. Not a sizeof full derived KGSL type.
  p.growStructure(0x20c0-p.getLength());
  // Two independently observed original info locations in init.
  if(getScriptArgs().length>1) {
   p.replaceAtOffset(0x1d90,info,info.getLength(),"embedded_original_info",null);
   p.replaceAtOffset(0x20b8,new PointerDataType(info),8,"info",null);
  }
  if(getScriptArgs().length>2)for(String row:Files.readAllLines(Paths.get(getScriptArgs()[2]))) {
   String[] x=row.split("\t",2);int offset=Integer.parseInt(x[0],16);
   p.replaceAtOffset(offset,BooleanDataType.dataType,1,"feature_"+x[1],"Original getter route, not a capability-validity claim");
  }
  if(getScriptArgs().length>3)for(String row:Files.readAllLines(Paths.get(getScriptArgs()[3]))) {
   String[] x=row.split("\t",5);int offset=Integer.parseInt(x[0],16),size=Integer.parseInt(x[1]),count=Integer.parseInt(x[3]);
   DataType element=x[2].equals("float")?FloatDataType.dataType:x[2].equals("double")?DoubleDataType.dataType:x[2].equals("char")?CharDataType.dataType:size/count==1?UnsignedCharDataType.dataType:size/count==8?UnsignedLongLongDataType.dataType:x[2].equals("int32_t")?IntegerDataType.dataType:UnsignedIntegerDataType.dataType;
   DataType dt=count==1?element:new ArrayDataType(element,count,element.getLength());
   if(dt.getLength()!=size)throw new Exception("Original property length mismatch: "+row);
   p.replaceAtOffset(offset,dt,size,"property_"+x[4],"Original full-field getter byte-copy route, not a supported capability claim");
  }
  Function init=currentProgram.getFunctionManager().getFunctionAt(toAddr(0xa87d7c));
  init.setReturnType(IntegerDataType.dataType,SourceType.USER_DEFINED);
  init.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
   new ParameterImpl("physical",new PointerDataType(p),currentProgram),
   new ParameterImpl("instance",new PointerDataType(VoidDataType.dataType),currentProgram));
  for(Function f:currentProgram.getFunctionManager().getFunctions(true)) {
   String n=f.getName();if(!n.equals("fd_dev_info")&&!n.equals("fd_dev_info_raw")&&!n.equals("fd_dev_name"))continue;
   DataType ret=n.equals("fd_dev_info")?info:new PointerDataType(n.equals("fd_dev_name")?CharDataType.dataType:info);
   f.setReturnType(ret,SourceType.USER_DEFINED);
   f.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
    new ParameterImpl("id",new PointerDataType(VoidDataType.dataType),currentProgram));
  }
  println("Typed original physical extension prefix and source-declared fd_dev_info structure return; no source bytes changed");
 }
}
