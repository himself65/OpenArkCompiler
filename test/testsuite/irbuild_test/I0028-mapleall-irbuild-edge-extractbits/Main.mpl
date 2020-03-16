 func $foo (
  var %i i32 
#var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     extractbits i32 1 23(
       constval i32 -998))}

 func $foo1 (
  var %i i32, var %j i32, var %k i32, 
  var %i1 i32, var %j1 i32, var %k1 i32
  ) i32 { 
   return (
     extractbits i32 0 5(dread i32 %i))}

 func $foo11 (
  var %i i32, var %j i32, var %k i32, 
  var %i1 i32, var %j1 i32, var %k1 i32
  ) u32 { 
   return (
     extractbits u32 0 5(dread i32 %i))}

 func $foo2 (
  var %i i32, var %j i32, var %k i32
  ) i32 { 
   return (
     extractbits i32 3 20(constval i32 0x111111111))}

 func $foo3 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     extractbits i64 5 50(dread i64 %i))}

 func $foo4 (
  var %i i64, var %j i64, var %k i32
  )i64  { 
   return (
     extractbits i64 6 44 (dread i64 %i))}

 func $foo5 (
  var %i i64, var %j i64, var %k i32
  ) i64 { 
   return (
     extractbits i64 1 10(constval i64 0x11111))}

 func $foo9 (
  var %i i8, var %j u8, var %k i32) u32 { 
   return (
     extractbits u32 1 31(dread u32 %j))}

 func $foo51 (
  var %i u64
  ) u64 { 
   return (
     extractbits u64 0 15(constval i64 0x11111))}

 func $foo52 (
  var %i i32
  ) i32 { 
   return (
     sext i32 11 (constval i32 99))}

 func $foo53 (
  var %i u64
  ) u64 { 
   return (
     zext u64 11 (constval u64 88))}

 func $foo54 (
  var %i i32
  ) i32 { 
   return (
     sext i32 11 (dread i32 %i))}

 func $foo55 (
  var %i u64
  ) u64 { 
   return (
     zext u64 41 (dread u64 %i))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
