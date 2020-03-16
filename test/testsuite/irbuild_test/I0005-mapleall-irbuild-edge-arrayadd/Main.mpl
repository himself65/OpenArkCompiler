var $arr1 <[10] i32>
var $arr2 <[10] i32>
var $arr3 <[10] i32>

func $ArrayAdd () void {
 var %i i32
 doloop %i ( 
  constval i32 0, 
  lt i32 i32 (dread i32 %i, constval i32 10), 
  add i32(dread i32 %i, constval i32 1)){
   iassign <* [10] i32>(
    array 1 a32 <* [10] i32>(addrof a32 $arr1, dread i32 %i),
    add i32(
     iread i32 <* [10] i32>(
      array 1 a32 <* [10] i32>(addrof a32 $arr2, dread i32 %i)), 
     iread i32 <* [10] i32>(
      array 1 a32 <* [10] i32>(addrof a32 $arr3, dread i32 %i))
     ))
  }
  return() 
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
