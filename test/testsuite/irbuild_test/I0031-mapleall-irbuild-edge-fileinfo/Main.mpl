fileinfo{ 
  @magic 345, 
  @filename "adir/thisisafile.dex",
  @dexoffset 560,
  @flags 0x01ef,
  @linksize 100
}
type $SS <class {
        @real f32,
        @imag f32}>
var $sconst1 <class {
        @classidx 22,
        @f11 i32,
        @f22 <$SS>,
        @f33 i32,
        @accessflag 0x45e,
        @classdataoffset 118,
        @f44 i32,
        @f55 f32,
        @f66 f32,
        @superclassid 3333
           }>
func $fact () i32 {
  funcinfo{ 
    @funcname "afunc",
    @registers 345, 
    @ins 560,
    @outs 444
  }
  return (dread i32 $sconst1 1)
  funcinfo{ 
    @funcidx 345, 
    @classidx 560
  }
}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
