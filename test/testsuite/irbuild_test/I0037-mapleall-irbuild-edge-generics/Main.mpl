type $ss1 <struct {@b <!T>,
                       @x <* f32>}>
type $ss2 <struct {@b1 <struct {@b <!SS>, 
                                    @x <* f32>}>,
                       @pa <* <struct {@b f64, 
                                       @x <* f32>}>>, 
                       @x1 <[10] f64>,
                       @dx <!U>}>
type $ss3 <[] <!AAA>>
func &foo (var %x <!W>) i32 { 
  var %y <!W>
  dassign %y (dread agg %x)
  return (dread i32 %y 1)
}
var $g <$ss1{!T=i32}>
var $h <$ss2{!U=i32,!SS=<$ss1{!T=f32}>}>
func &bar () i32 {
  var %i i32
  callinstant &foo<{!W=<$ss2{!U=f32,!SS=i32}>}>( dread agg $g )
  callinstantassigned &foo<{!W=<$ss1{!T=i32}>}>( dread agg $g ) { dassign %i 0 }
  virtualcallinstant &foo<{!W=<$ss2{!U=f32,!SS=i32}>}>( dread agg $g )
  virtualcallinstantassigned &foo<{!W=<$ss1{!T=i32}>}>( dread agg $g ) { dassign %i 0 }
  superclasscallinstant &foo<{!W=<$ss2{!U=f32,!SS=i32}>}>( dread agg $g )
  superclasscallinstantassigned &foo<{!W=<$ss1{!T=i32}>}>( dread agg $g ) { dassign %i 0 }
  interfacecallinstant &foo<{!W=<$ss2{!U=f32,!SS=i32}>}>( dread agg $g )
  interfacecallinstantassigned &foo<{!W=<$ss1{!T=i32}>}>( dread agg $g ) { dassign %i 0 }
  return ( dread i32 %i )
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
