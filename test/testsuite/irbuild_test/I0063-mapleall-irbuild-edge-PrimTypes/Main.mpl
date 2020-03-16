var $myStruct < struct{
    @number i32,
    @p <* i8> }>
func $main ()i32{
     #Character
     var %sChar i8
     var %usChar i8
     #signed and unsigned short integer
     var %shortInt i16
     var %shortInt2 i16
     var %sShortInt i16
     var %sShortInt2 i16
     var %usShortInt i16
     var %usShortInt2 i16
     #signed and unsigned integer
     var %int1 i32
     var %sInt i32
     var %usInt i32
     var %usInt2 i32
     #signed and unsigned long integer
     var %long1 i64
     var %long2 i64
     var %sLong1 i64
     var %sLong2 i64
     var %usLong i64
     var %usLong2 i64
     var %lLong1 i64
     var %iLong2 i64
     var %sLlong1 i64
     var %sLlonog2 i64
     var %usLlong i64
     var %usLlong2 i64
     #float
     var %Float f32
     #double
     var %Double f64
     #long double
     var %lDouble f64
     #Address
     var %array <[10] i8>
     iassign<* i8>(
      array 1 a64<*[10] i8>(addrof a64 %array, constval i64 1),
      constval i32 120)}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
