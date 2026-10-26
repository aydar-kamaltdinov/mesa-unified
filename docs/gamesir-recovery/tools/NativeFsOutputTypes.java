// Read-only native field naming; partial structures, never sizeof/ABI claims.
// The caller must use the matching GameSir ELF with -readOnly -noanalysis.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;
public class NativeFsOutputTypes extends GhidraScript {
  void field(StructureDataType s,int o,DataType t,String name) {
    s.replaceAtOffset(o,t,t.getLength(),name,"Exact original field offset; partial view only");
  }
  public void run() throws Exception {
    CategoryPath cat=new CategoryPath("/GameSirOriginalFsOutputFields");
    StructureDataType output=new StructureDataType(cat,"native_ir3_output_record",5);
    field(output,0,UnsignedCharDataType.dataType,"slot");
    field(output,1,UnsignedCharDataType.dataType,"regid");
    field(output,2,UnsignedCharDataType.dataType,"view");
    field(output,3,UnsignedCharDataType.dataType,"aliased_components");
    field(output,4,BooleanDataType.dataType,"half");
    StructureDataType variant=new StructureDataType(cat,"native_ir3_variant_prefix_partial",0x3c0);
    field(variant,0xb8,UnsignedIntegerDataType.dataType,"stage");
    field(variant,0x302,BooleanDataType.dataType,"dual_src_blend");
    field(variant,0x309,BooleanDataType.dataType,"color0_mrt");
    field(variant,0x30c,UnsignedIntegerDataType.dataType,"outputs_count");
    field(variant,0x310,new ArrayDataType(output,34,5),"outputs");
    String[] names={"writes_pos","writes_smask","writes_psize","writes_viewport","writes_stencilref","writes_shading_rate"};
    for(int i=0;i<names.length;i++) field(variant,0x3ba+i,BooleanDataType.dataType,names[i]);
    DataType pvariant=new PointerDataType(variant);
    StructureDataType ctx=new StructureDataType(cat,"native_ir3_context_prefix_partial",0x50);
    field(ctx,0x28,pvariant,"so");
    FunctionManager fm=currentProgram.getFunctionManager();
    for(long a:new long[]{0xa13dbcL,0xa15830L,0xa173f8L}) {
      Function fn=fm.getFunctionAt(toAddr(a+0x100000));
      if(fn==null)throw new Exception("Missing exact FS output function");
      fn.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);
      fn.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
        new ParameterImpl("cs",new PointerDataType(VoidDataType.dataType),currentProgram),
        new ParameterImpl("fs",pvariant,currentProgram));
    }
    Function setup=fm.getFunctionAt(toAddr(0xbc1e10));
    setup.setReturnType(VoidDataType.dataType,SourceType.USER_DEFINED);
    setup.replaceParameters(Function.FunctionUpdateType.DYNAMIC_STORAGE_ALL_PARAMS,true,SourceType.USER_DEFINED,
      new ParameterImpl("ctx",new PointerDataType(ctx),currentProgram),
      new ParameterImpl("intr",new PointerDataType(VoidDataType.dataType),currentProgram));
    println("Named exact FS output record fields only; partial struct extents are NOT original ABI sizes");
  }
}
