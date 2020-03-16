type $SS <struct {
        @real f32,
        @imag f32}>
var $sconst1 <struct {
        @f11 i32,
        @f22 <$SS>,
        @f33 i32,
        @f44 i32,
        @f55 f32,
        @f66 f32}> = [ 1=1007, 2= [1=11.11f, 2=22.22f], 5=-273, 6=75, 4=6023.1f ]
func $fact () i32 {
  return (dread i32 $sconst1 1)
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
