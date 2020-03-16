type $SS <struct {
        @real f32,
        @imag f32}>
var $sconst1 <struct {
        @f11 i32,
        @f22 <$SS>,
        @f33 i32,
        @f44 i32,
        @f55 f32,
        @f66 f32}> = [ 6=1007, 2= [1=11.11f, 2=22.22f], 3=-273.2f, 4=75.3f, 1=1425926, 5=6023 ]
func $printf (var %p1 <* i8>)void
func $main ( ) i32 {
   call &printf (addrof a32 $sconst1)
   return (constval i32 0) }



 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
