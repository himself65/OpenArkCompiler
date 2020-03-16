var $arr1 <[10] f32>

func $ArrayAdd () f32 {
 var %ff i32
 var %sum f32
 dassign %sum ( constval f32 0.0f )
 foreachelem %ff $arr1 { 
   dassign %sum ( add f32 ( dread f32 %sum, dread f32 %ff ) )
 }
 return ( dread f32 %sum ) 
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
