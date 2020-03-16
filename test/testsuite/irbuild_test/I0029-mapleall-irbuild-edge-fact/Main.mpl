#int fact(int n) {
#  if (n == 1)
#    return 1;
#  return n * fact(n-1);
#}

func $fact (var %n i32) i32 {
  if (ne i32 i32 (dread i32 %n, constval i32 1)) {
       call &fact (sub i32 (dread i32 %n, constval i32 1))
            return (mul i32 (dread i32 %n, regread i32 %%retval)) 
             }
  return (constval i32 1) 
}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
