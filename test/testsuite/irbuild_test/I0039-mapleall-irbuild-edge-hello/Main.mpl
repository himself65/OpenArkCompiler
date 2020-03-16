var $const1 <[13] i8> = [ 104, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 10, 0 ]
func $printf (var %p1 <* i8>)void
func $main ( ) i32 {
   call &printf (addrof a32 $const1)
   return (constval i32 0) }



 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
