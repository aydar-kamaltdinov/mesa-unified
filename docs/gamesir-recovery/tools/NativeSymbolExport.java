// Static decompilation only. Run on the recovery project copy with -readOnly.
// Args: jobs.tsv output-directory [threads]. Bound bodies by ELF symbol size.
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.address.*;
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.util.task.TaskMonitor;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.io.*;

public class NativeSymbolExport extends GhidraScript {
  public void run() throws Exception {
    String[] args = getScriptArgs();
    Path dest = Paths.get(args[1]); Files.createDirectories(dest);
    int threads = args.length > 2 ? Integer.parseInt(args[2]) : 2;
    int timeout = args.length > 3 ? Integer.parseInt(args[3]) : 90;
    boolean conservative = args.length > 4 && args[4].equals("conservative");
    String simplification = args.length > 7 ? args[7] : "decompile";
    if (!simplification.equals("decompile") && !simplification.equals("normalize"))
      throw new IOException("Only decompile/normalize analysis actions are accepted");
    FunctionManager fm = currentProgram.getFunctionManager();
    // The matching Mesa declaration is NORETURN. The optimized release
    // implementation ends in an unreachable fallthrough after logging;
    // without this contract Ghidra merges following functions into callers.
    // ir3_assert_handler aborts/longjmps; _vtn_fail is also declared NORETURN.
    // Without those source contracts a failed-assert tail can absorb the next
    // ELF symbol even when its nominal function body was bounded.
    for (String noReturnName : new String[]{"ir3_context_error", "ir3_assert_handler", "_vtn_fail"}) {
      for (ghidra.program.model.symbol.Symbol symbol :
           currentProgram.getSymbolTable().getSymbols(noReturnName)) {
        Function errorFunction=fm.getFunctionAt(symbol.getAddress());
        if (errorFunction != null) {
          errorFunction.setNoReturn(true);
          println("Analysis contract: " + noReturnName + " is NORETURN");
        }
      }
    }
    // Optional ELF-resolved PLT contracts; addresses must come from the
    // matching binary's dynsym/rela.plt, never from another build.
    if (args.length > 5) {
      for (String line : Files.readAllLines(Paths.get(args[5]))) {
        String[] p = line.split("\\t", 2);
        Address address = toAddr(Long.parseUnsignedLong(p[0], 16));
        Function fn = fm.getFunctionAt(address);
        if (fn == null) {
          disassemble(address);
          new CreateFunctionCmd(address).applyTo(currentProgram, monitor);
          fn = fm.getFunctionAt(address);
        }
        if (fn == null) throw new IOException("No PLT function at " + line);
        fn.setNoReturn(true);
        println("ELF-resolved analysis contract: " + line + " is NORETURN");
      }
    }
    // Optional positive stack-restoring direct-tailcall contracts derived
    // from this exact ELF. Flow overrides are analysis-only in readOnly mode.
    if (args.length > 6) {
      for (String line : Files.readAllLines(Paths.get(args[6]))) {
        String[] p = line.split("\\t", 2);
        Address address = toAddr(Long.parseUnsignedLong(p[0], 16));
        disassemble(address);
        Instruction tail = getInstructionAt(address);
        if (tail == null || !tail.getMnemonicString().equalsIgnoreCase("b"))
          throw new IOException("Expected direct B at " + line);
        tail.setFlowOverride(FlowOverride.CALL_RETURN);
      }
      println("Applied ELF positive stack-restoring direct tailcall contracts");
    }
    List<Object[]> jobs = new ArrayList<>();
    PrintWriter prep = new PrintWriter(dest.resolve("prepare.tsv").toFile());
    prep.println("address\tsize\tname\tstatus");
    for (String line : Files.readAllLines(Paths.get(args[0]))) {
      String[] p = line.split("\t",3);
      Address address = toAddr(Long.parseUnsignedLong(p[0],16));
      long size = Long.parseUnsignedLong(p[1],16);
      try {
        Function fn = fm.getFunctionAt(address);
        if (fn == null) {
          disassemble(address);
          new CreateFunctionCmd(address).applyTo(currentProgram, monitor);
          fn = fm.getFunctionAt(address);
        }
        if (fn == null) { prep.println(line+"\tno-function"); continue; }
        AddressSet body = new AddressSet(address,address.add(size-1));
        fn.setBody(body);
        if (conservative && size < 128 && p[2].startsWith("_Z23tu_cmd_fill_buffer_addr")) {
          disassemble(address.add(size-4));
          Instruction tail = getInstructionAt(address.add(size-4));
          if (tail != null && tail.getMnemonicString().equalsIgnoreCase("br")) {
            // This is the gen-dispatch tail-call thunk, not a switch body.
            // Analysis-only override: no instruction bytes are changed.
            tail.setFlowOverride(FlowOverride.CALL_RETURN);
            prep.println(line+"\tanalysis-tail-call-override");
          }
        }
        jobs.add(new Object[]{fn,p[0],p[2]});
        prep.println(line+"\tbounded");
      } catch(Exception e) { prep.println(line+"\tERROR "+e.toString()); }
    }
    prep.close();
    println("Prepared "+jobs.size()+" functions; starting static decompilation");
    ConcurrentLinkedQueue<Object[]> queue = new ConcurrentLinkedQueue<>(jobs);
    AtomicInteger counter = new AtomicInteger();
    PrintWriter report = new PrintWriter(dest.resolve("results.tsv").toFile());
    report.println("address\tname\tstatus\tfile");
    List<Thread> workers = new ArrayList<>();
    for(int i=0;i<threads;i++) {
      Thread worker = new Thread(() -> {
        DecompInterface di = new DecompInterface();
        if (conservative) {
          DecompileOptions options=new DecompileOptions();
          options.setPredicate(false);
          options.setSplitStructures(false);
          options.setSplitArrays(false);
          options.setSplitPointers(false);
          options.setSimplifyDoublePrecision(false);
          di.setOptions(options);
        }
        di.setSimplificationStyle(simplification);
        if (!di.openProgram(currentProgram)) {
          synchronized(report) {
            report.println("OPEN_FAILED\t"+di.getLastMessage());report.flush();
          }
          di.dispose();return;
        }
        Object[] job;
        while((job=queue.poll())!=null) {
          String name=(String)job[2], address=(String)job[1];
          String file=address+"-"+name.replaceAll("[^A-Za-z0-9_]","_");
          if(file.length()>190) file=file.substring(0,190);
          file += ".c";
          String status;
          try {
            DecompileResults r=di.decompileFunction((Function)job[0],timeout,TaskMonitor.DUMMY);
            status=r.decompileCompleted()?"completed":"FAILED "+r.getErrorMessage();
            if (simplification.equals("normalize") && r.getHighFunction() != null) {
              // normalize produces HighFunction SSA, not decompiled C. Keep
              // a distinct extension/status so it cannot be counted as C.
              file = file.substring(0, file.length() - 2) + ".pcode.tsv";
              try (PrintWriter pc = new PrintWriter(dest.resolve(file).toFile())) {
                pc.println("// normalized SSA only; not decompiled C or source-equivalence proof");
                var ops = r.getHighFunction().getPcodeOps();
                while (ops.hasNext()) {
                  var op = ops.next();
                  pc.println(Long.toHexString(op.getSeqnum().getTarget().getOffset() - 0x100000) + "\t" + op);
                }
              }
              status = "normalized-ssa";
            } else {
              boolean hasC = r.decompileCompleted() && r.getDecompiledFunction() != null;
              if (r.decompileCompleted() && !hasC) status = "FAILED no C output";
              String code = hasC ? r.getDecompiledFunction().getC() : "// " + status;
              Files.writeString(dest.resolve(file),"// "+name+" @ "+address+"\n"+code,
                                StandardCharsets.UTF_8);
            }
          } catch(Exception e) { status="ERROR "+e.toString(); }
          status=status.replace('\n',' ').replace('\r',' ').replace('\t',' ');
          synchronized(report) { report.println(address+"\t"+name+"\t"+status+"\t"+file); report.flush(); }
          if (!status.equals("completed") && !status.equals("normalized-ssa")) {
            di.dispose(); di=new DecompInterface();
            if (conservative) {
              DecompileOptions options=new DecompileOptions();
              options.setPredicate(false);
              options.setSplitStructures(false);
              options.setSplitArrays(false);
              options.setSplitPointers(false);
              options.setSimplifyDoublePrecision(false);
              di.setOptions(options);
            }
            di.setSimplificationStyle(simplification);
            if (!di.openProgram(currentProgram)) {
              synchronized(report) { report.println("OPEN_FAILED\t"+di.getLastMessage());report.flush(); }
              break;
            }
          }
          int count=counter.incrementAndGet();
          if(count%200==0) System.out.println("NativeSymbolExport progress "+count+"/"+jobs.size());
        }
        di.dispose();
      });
      worker.start();workers.add(worker);
    }
    for(Thread worker:workers) worker.join();
    report.close();println("NativeSymbolExport done "+counter.get());
  }
}
