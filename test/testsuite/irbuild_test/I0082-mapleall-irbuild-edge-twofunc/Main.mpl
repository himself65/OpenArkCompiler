#int fact (int n) {
#  if(n != 1)
#    return foo(n - 1);
#  else return 1;
#}
func $foo varargs ( var %i i32, ... ) i32 {
  return (dread i32 %i) }
func $foo1 varargs ( ... ) i32 {
  return (constval i32 99) }

func $fact (
 var %n i32) i32 {
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
    call &foo(
      sub i32 (dread i32 %n, constval i32 1))
    return (regread i32 %%retval)}
   else {
     return (constval i32 1) } }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
