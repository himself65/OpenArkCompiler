# ;, / and | are allowed name characters, but they cannot be the starting char
 func $foo (
  var %i; i32,
  var %j|/ i32 
  ) i32 { 
   return (
     add i32(dread i32 %i;, dread i32 %j|/))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
