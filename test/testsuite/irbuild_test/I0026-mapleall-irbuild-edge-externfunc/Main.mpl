func $foo ( var %i i32 ) i32 
func $fact ( var %n i32) i32 
func $fact ( var %n i32) i32 
func $foo ( var %i i32 ) i32 

func $fact (
 var %n i32) i32 {
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
    call &foo(
      sub i32 (dread i32 %n, constval i32 1))
    return (regread i32 %%retval)}
   else {
     return (constval i32 1) } }

func $foo ( var %i i32 ) i32 {
  return (dread i32 %i) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
