// Bounded original SSA for physical-device extension stores. No execution.
// Each definition has its own id; reused decompiler C variable names are not identities.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.program.model.pcode.*;
import java.nio.file.*;
import java.io.*;
import java.util.*;
public class NativeExtensionStoreGraph extends GhidraScript {
 static class E {String root;long offset; E(String r,long o){root=r;offset=o;} boolean constant(){return root==null;}}
 Map<Varnode,E> cache=new HashMap<>(); Set<Varnode> busy=new HashSet<>();
 Map<Varnode,Integer> ids=new HashMap<>();ArrayList<Varnode> nodes=new ArrayList<>();
 Address physical,instance;
 String callee(PcodeOp d) {if(d.getOpcode()!=PcodeOp.CALL)return "";Function f=currentProgram.getFunctionManager().getFunctionAt(d.getInput(0).getAddress());return f==null?d.getInput(0).toString():f.getName();}
 E combine(E a,E b){if(a==null||b==null||!a.constant()&&!b.constant())return null;return new E(a.constant()?b.root:a.root,a.offset+b.offset);}
 E ptr(Varnode n,int depth) {
  if(n==null||depth>90)return null;
  if(n.isConstant())return new E(null,n.getOffset());
  if(n.isInput()&&n.getAddress().equals(physical))return new E("physical",0);
  if(n.isInput()&&n.getAddress().equals(instance))return new E("instance",0);
  if(cache.containsKey(n))return cache.get(n);if(!busy.add(n))return null;
  PcodeOp d=n.getDef();E a,b,c,result=null;
  if(d!=null)switch(d.getOpcode()) {
   case PcodeOp.COPY:case PcodeOp.CAST:result=ptr(d.getInput(0),depth+1);break;
   case PcodeOp.INT_ADD:case PcodeOp.PTRSUB:result=combine(ptr(d.getInput(0),depth+1),ptr(d.getInput(1),depth+1));break;
   case PcodeOp.PTRADD:
    a=ptr(d.getInput(1),depth+1);b=ptr(d.getInput(2),depth+1);
    if(a!=null&&b!=null&&a.constant()&&b.constant())result=combine(ptr(d.getInput(0),depth+1),new E(null,a.offset*b.offset));break;
   case PcodeOp.MULTIEQUAL:
    for(int i=0;i<d.getNumInputs();i++) {a=ptr(d.getInput(i),depth+1);if(a==null){result=null;break;}if(i==0)result=a;else if(!Objects.equals(a.root,result.root)||a.offset!=result.offset){result=null;break;}}break;
   case PcodeOp.LOAD:
    a=ptr(d.getInput(1),depth+1);
    if(a!=null&&a.root!=null)result=new E("load("+a.root+"+0x"+Long.toHexString(a.offset)+")",0);break;
   case PcodeOp.CALL:result=new E("return("+callee(d)+"@"+Long.toHexString(d.getSeqnum().getTarget().getOffset()-0x100000)+")",0);break;
  }
  busy.remove(n);cache.put(n,result);return result;
 }
 int node(Varnode n){Integer id=ids.get(n);if(id!=null)return id;int i=nodes.size();ids.put(n,i);nodes.add(n);return i;}
 public void run() throws Exception {
  Path out=Paths.get(getScriptArgs()[0]);Files.createDirectories(out);
  long rangeStart=getScriptArgs().length>1?Long.parseLong(getScriptArgs()[1],16):0x58;
  long rangeEnd=getScriptArgs().length>2?Long.parseLong(getScriptArgs()[2],16):0x207;
  Function fn=currentProgram.getFunctionManager().getFunctionAt(toAddr(0xa87d7c));
  fn.setBody(new AddressSet(fn.getEntryPoint(),fn.getEntryPoint().add(8800-1)));
  physical=fn.getParameter(0).getVariableStorage().getFirstVarnode().getAddress();instance=fn.getParameter(1).getVariableStorage().getFirstVarnode().getAddress();
  DecompInterface di=new DecompInterface();DecompileOptions opts=new DecompileOptions();opts.setPredicate(false);opts.setSplitStructures(false);opts.setSplitArrays(false);opts.setSplitPointers(false);opts.setSimplifyDoublePrecision(false);di.setOptions(opts);di.openProgram(currentProgram);
  DecompileResults result=di.decompileFunction(fn,180,monitor);
  if(!result.decompileCompleted()||result.getHighFunction()==null)throw new Exception("No high function: "+result.getErrorMessage());
  try(PrintWriter stores=new PrintWriter(out.resolve("extension-stores.tsv").toFile());PrintWriter graph=new PrintWriter(out.resolve("value-nodes.tsv").toFile());PrintWriter calls=new PrintWriter(out.resolve("extension-memory-calls.tsv").toFile())) {
   stores.println("elf_instruction\tphysical_offset\textension_index\tbytes\tvalue_node");
   calls.println("elf_instruction\tcallee\targument\tphysical_offset\tinput_nodes");
   var ops=result.getHighFunction().getPcodeOps();int total=0;
   while(ops.hasNext()) {
    PcodeOpAST op=ops.next();long pc=op.getSeqnum().getTarget().getOffset()-0x100000;
    if(pc<0x987d7c||pc>=0x987d7c+8800)continue;
    if(op.getOpcode()==PcodeOp.STORE) {
     E at=ptr(op.getInput(1),0);
     if(at!=null&&"physical".equals(at.root)&&at.offset<rangeEnd&&at.offset+op.getInput(2).getSize()>rangeStart){stores.println(Long.toHexString(pc)+"\t"+Long.toHexString(at.offset)+"\t"+(at.offset-rangeStart)+"\t"+op.getInput(2).getSize()+"\t"+node(op.getInput(2)));total++;}
    }else if(op.getOpcode()==PcodeOp.CALL) {
     for(int i=1;i<op.getNumInputs();i++){E at=ptr(op.getInput(i),0);if(at!=null&&"physical".equals(at.root)&&at.offset>=rangeStart-2&&at.offset<rangeEnd){StringJoiner ins=new StringJoiner(",");for(int j=1;j<op.getNumInputs();j++)ins.add(Integer.toString(node(op.getInput(j))));calls.println(Long.toHexString(pc)+"\t"+callee(op)+"\t"+i+"\t"+Long.toHexString(at.offset)+"\t"+ins);}}
    }
   }
   try(PrintWriter blocks=new PrintWriter(out.resolve("control-blocks.tsv").toFile())) {
    blocks.println("block\tstart_elf\tstop_elf\tpredecessors\tsuccessors\tfalse_successor\ttrue_successor\tterminator\tcondition_node");
    for(PcodeBlockBasic block:result.getHighFunction().getBasicBlocks()) {
     StringJoiner predecessors=new StringJoiner(","),successors=new StringJoiner(",");
     for(int k=0;k<block.getInSize();k++)predecessors.add(Integer.toString(block.getIn(k).getIndex()));
     for(int k=0;k<block.getOutSize();k++)successors.add(Integer.toString(block.getOut(k).getIndex()));
     PcodeOp last=block.getLastOp();String condition="",f="",t="";
     if(last!=null&&last.getOpcode()==PcodeOp.CBRANCH&&block.getOutSize()==2) {
      condition=Integer.toString(node(last.getInput(1)));f=Integer.toString(block.getFalseOut().getIndex());t=Integer.toString(block.getTrueOut().getIndex());
     }
     blocks.println(block.getIndex()+"\t"+Long.toHexString(block.getStart().getOffset()-0x100000)+"\t"+Long.toHexString(block.getStop().getOffset()-0x100000)+"\t"+predecessors+"\t"+successors+"\t"+f+"\t"+t+"\t"+(last==null?"":last.getMnemonic())+"\t"+condition);
    }
   }
   graph.println("node\tsize\tdefinition\telf_instruction\tinputs\tvarnode\tpointer_expression\tphi_predecessors");
   for(int i=0;i<nodes.size();i++) {
    if(nodes.size()>20000)throw new Exception("Unexpected graph expansion");Varnode n=nodes.get(i);PcodeOp d=n.getDef();StringJoiner ins=new StringJoiner(",");
    String kind=n.isConstant()?"CONST_0x"+Long.toHexString(n.getOffset()):n.isInput()?"INPUT":"UNDEFINED";String pc="";
    if(d!=null){kind=d.getMnemonic();pc=Long.toHexString(d.getSeqnum().getTarget().getOffset()-0x100000);if(d.getOpcode()==PcodeOp.CALL)kind+="_"+callee(d);for(int j=0;j<d.getNumInputs();j++)ins.add(Integer.toString(node(d.getInput(j))));}
    E p=ptr(n,0);String expr=p==null?"unresolved":p.constant()?"0x"+Long.toHexString(p.offset):p.root+"+0x"+Long.toHexString(p.offset);
    StringJoiner phi=new StringJoiner(",");
    if(d!=null&&d.getOpcode()==PcodeOp.MULTIEQUAL) {
     PcodeBlockBasic parent=((PcodeOpAST)d).getParent();
     if(parent.getInSize()!=d.getNumInputs())throw new Exception("Phi predecessor mismatch");
     for(int k=0;k<parent.getInSize();k++)phi.add(Integer.toString(parent.getIn(k).getIndex()));
    }
    graph.println(i+"\t"+n.getSize()+"\t"+kind+"\t"+pc+"\t"+ins+"\t"+n+"\t"+expr+"\t"+phi);
   }
   println("Original selected-field stores="+total+"; value graph nodes="+nodes.size()+"; branch/path feasibility not inferred");
  } finally{di.dispose();}
 }
}
