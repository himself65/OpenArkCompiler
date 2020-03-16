var $nan_f32 <f32> = nanf
var $nan_f64 <f64> = nan
var $inf_f32 <f32> = -inff
var $inf_f64 <f64> = inf
var $g_f32 <f32> = -1.2f
var $g_f64 <f64> = 1.2
func $add_f32 () f32 {
  return (
    add f32(dread f32 $g_f32, constval f32 inff))}
func $add_f64 () f64 {
  return (
    add f64(dread f64 $g_f64, constval f64 -inf))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
