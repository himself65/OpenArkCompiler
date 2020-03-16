var $sconst1 <struct {
        @f11 i32,
        @f22:15 i32,
        @f33 i32,
        @f44 i32,
        @f55 f32,
        @f66 f32}> = [ 6=10.07f, 2=699, 3=-273, 4=75, 1=31425926, 5=6.02e+23f ]
func &printf (var %p1 <* i8>)void
func &main ( ) i32 {
   call &printf (addrof a32 $sconst1)
   return (constval i32 0) }



 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
