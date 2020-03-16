var $p <* i32>
func $Foo (var %i i32) void {
 iassign <* i32>(
    dread a64 $p,
    add i32(
     iread i32 <* i32> ( dread a64 $p ), 
     dread i32 %i))
  return() 
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
