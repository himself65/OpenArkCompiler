 # a function returning (i + j) * -998
 func $foo (
# var %i xxx,
  var %i i32, var %j i32, var %k i32, 
  var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 {
   assertge (dread i32 %i, constval i32 0)
   assertlt (dread i32 %i, dread i32 %k)
   return (
     ashr i32(
      mul i32 (
       add i32 ( 
        band i32 (dread i32 %i, dread i32 %j),
        bior i32 ( 
         bxor i32(dread i32 %i1, dread i32 %k1), 
         eq i32 i32 (
          depositbits i32 1 23 (
           land i32 (dread i32 %j1, dread i32 %k), 
           lior i32 (
             min i32 (
              shl i32 (dread i32 %k1, dread i32 %j), 
              sub i32 (dread i32 %j1, dread i32 %i)), 
             rem i32 (dread i32 %i, dread i32 %k))), 
          div i32( 
           lshr i32 ( dread i32 %k, dread i32 %i), 
           max i32 ( dread i32 %i, dread i32 %j))))), 
       constval i32 -998), 
      dread i32 %i))}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
