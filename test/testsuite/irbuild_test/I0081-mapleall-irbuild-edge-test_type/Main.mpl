func $foo1 (var %p1 i8, var %p2 i16, var %p3 i32, var %p4 i64, ...)i32 {
   return (
       constval i32 33)}
func $foo2 (var %p1 u8, var %p2 u16, var %p3 u32, var %p4 u64,...) i32 {
   return (
       constval i32 33)}
func $foo4 (var %p1 f32, var %p2 f64, ... ) i32 {
   return (
       constval i32 33)}

func $foo5 (var %p1 a32, var %p2 a64,...) i32 
func $foo6 (var %p1 c64, var %p2 c128, ...) i32

func $foo7 (var %p1 <* i8>, var %p2 <* i16>, var %p3 <* i32>, var %p4 <* i64>) i32 {
   return (
       constval i32 33)}
func $foo8 (var %p1 <* u8>, var %p2 <* u16>, var %p3 <* u32>, var %p4 <* u64>) i32 {
   return (
       constval i32 33)}
func $foo9 (var %p1 <* u1>) i32 {
   return (
       constval i32 33)}
func $foo10 (var %p1 <* f32>, var %p2 <* f64>) i32 {
   return (
       constval i32 33)}
func $foo11 (var %p1 <* void>)void

func $foo12 (var %p1 <[10] i8>) i32 {
   return (
       constval i32 33)}
func $foo13 (var %p1 <[10][32] i32>) i32 {
   return (
       constval i32 33)}
func $foo14 (var %p1 <[10][32][43] i32>) i32 {
   return (
       constval i32 33)}
func $foo15 (var %p1 <[10][32][43][54][56][78][90][122][223][334] i32>) i32 {
   return (
       constval i32 33)}

type $SS <struct {@f1 i32, @f2:3 i8, @f3:5 i8}>
func $foo16 (var %x <$SS>) i32 {
   return (
       constval i32 33)}



 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
