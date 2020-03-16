#var $const1 <[13] i8> [ 104, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 10, 0 ]
var $fconst1 <[18] f64> = [ 1007, 707, -273, 75, 0113, 0x4b, 75u, 75l, 75ul, 75lu, 3.1425926, 6.02e23, 6.02e+23, 1.6e-19, 3.0, 3.14159, 6.02e23, .233 ]
func $printf (var %p1 <* i8>)void
func $main ( ) i32 {
   call &printf (addrof a32 $fconst1)
   call &printf (conststr ptr "\"hello maple\"", conststr a32 "\nworld\n")
   # 你好: E4 BD A0 E5 A5 BD  中国: E4 B8 AD E5 9B BD
   call &printf (conststr16 ptr "\xE4\xBD\xA0\xE5\xA5\xBD", conststr16 a32 "\xE4\xB8\xAD\xE5\x9B\xBD")
   return (constval i32 0) }



 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
