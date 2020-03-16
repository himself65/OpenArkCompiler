 # a function returning (i + j) * -998
 func $foo (
# var %i xxx,
  var %i i32,
   var %j i32) i32 { 
   return (
     mul i32 (
       add i32 ( dread i32 %i, dread i32 %j), 
       constval i32 -998))}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
