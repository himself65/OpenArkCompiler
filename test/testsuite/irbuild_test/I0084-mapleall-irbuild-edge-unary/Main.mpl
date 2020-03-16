 # a function returning unary(i + j) * -998
 func $foo (
  var %i i32,
   var %j i32) f32 { 
   return (
     recip f32(
       sqrt f32(  
      cvt f32 i32(
        mul i32 (
         extractbits i32 1 23 ( add i32 ( bnot i32 (dread i32 %i), lnot i32 (dread i32 %j))), 
         neg i32 ( constval i32 -998))))))}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
