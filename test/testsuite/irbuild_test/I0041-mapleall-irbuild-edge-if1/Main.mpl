#int fact (int n) {
#  if(n != 1)
#    return foo(n - 1);
#  else return 1;
#}

func $fact (
 var %n i32) i32 {
 if (ne i32 i32 (dread i32 %n, constval i32 1)) {
     return (constval i32 0) } }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
