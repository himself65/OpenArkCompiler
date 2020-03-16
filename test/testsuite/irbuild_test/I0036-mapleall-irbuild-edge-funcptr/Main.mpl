func $fact (
 var %n i32) i32 {
 var %foo <func(i32) i32>
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
    icall ( dread a64 %foo,
      sub i32 (dread i32 %n, constval i32 1))
    return (regread i32 %%retval)}
   else {
     return (constval i32 1) } }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
