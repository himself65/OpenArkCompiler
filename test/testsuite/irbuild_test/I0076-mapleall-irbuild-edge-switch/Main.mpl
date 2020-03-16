func $switchfunc ( var %n i32) i32 {
 var %alocal i32
 switch (dread i32 %n) @labdft {
   -3: goto @lab0
   1: goto @lab1
   9: goto @lab9 }
@lab0
 return (constval i32 -3)
@labdft
 return (constval i32 100)
@lab9
 return (constval i32 9)
@lab1
 return (constval i32 1) }
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
