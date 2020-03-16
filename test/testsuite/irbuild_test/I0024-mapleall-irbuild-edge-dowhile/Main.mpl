func $foo (var %i i32) i32 {
  dowhile {
    dassign %i (
      mul i32 (dread i32 %i, dread i32 %i))
    dassign %i (
      add i32 (dread i32 %i, constval i32 1) )} (
        lt i32 i32 ( dread i32 %i, constval i32 10) )
  return ( constval i32 0 )
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
