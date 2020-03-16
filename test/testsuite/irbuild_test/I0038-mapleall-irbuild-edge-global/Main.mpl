var $g_i8_u <i8>
var $g_i8 <i8> = 8
var $g_u8_u <u8>
var $g_u8 <u8> = 8

var $g_i16_u <i16>
var $g_i16 <i16> = 16
var $g_u16_u <u16>
var $g_u16 <u16> = 16

var $g_i32_u <i32>
var $g_i32 <i32> = 32
var $g_u32_u <u32>
var $g_u32 <u32> = 32

var $g_i64_u <i64>
var $g_i64 <i64> = 0x123456789a
var $g_u64_u <u64>
var $g_u64 <u64> = 0x123456789a

var $g_f32_u <f32>
var $g_f32 <f32> = 1.2f

var $g_f64_u <f64>
var $g_f64 <f64> = 2.4

# i8/u8
func $add_i8_u () i8 { 
   return (
     add i32(dread i32 $g_i8_u, constval i32 0))}

func $add_i8 () i8 { 
   return (
     add i32(dread i32 $g_i8, constval i32 0))}
	 
func $add_u8_u () u8 { 
   return (
     add u32(dread u32 $g_u8_u, constval u32 0))}

func $add_u8 () u8 { 
   return (
     add u32(dread u32 $g_u8, constval u32 0))}
	 
# i16/u16
func $add_i16_u () i16 { 
   return (
     add i32(dread i32 $g_i16_u, constval i32 0))}

func $add_i16 () i16 { 
   return (
     add i32(dread i32 $g_i16, constval i32 0))}
	 
func $add_u16_u () u16 { 
   return (
     add u32(dread u32 $g_u16_u, constval u32 0))}

func $add_u16 () u16 { 
   return (
     add u32(dread u32 $g_u16, constval u32 0))}
	 
# i32/u32
func $add_i32_u () i32 { 
   return (
     add i32(dread i32 $g_i32_u, constval i32 0))}

func $add_i32 () i32 { 
   return (
     add i32(dread i32 $g_i32, constval i32 0))}
	 
func $add_u32_u () u32 { 
   return (
     add u32(dread u32 $g_u32_u, constval u32 0))}

func $add_u32 () u32 { 
   return (
     add u32(dread u32 $g_u32, constval u32 0))}
	 
# i64/u64
func $add_i64_u () i64 { 
   return (
     add i64(dread i64 $g_i64_u, constval i64 0))}

func $add_i64 () i64 { 
   return (
     add i64(dread i64 $g_i64, constval i64 0))}
	 
func $add_u64_u () u64 { 
   return (
     add u64(dread u64 $g_u64_u, constval u64 0))}

func $add_u64 () u64 { 
   return (
     add u64(dread u64 $g_u64, constval u64 0))}
	 
# # f32
# func $add_f32_u () f32 { 
   # return (
     # add f32(dread f32 $g_f32_u, constval f32 0.0f))}

# func $add_f32 () f32 { 
   # return (
     # add f32(dread f32 $g_f32, constval f32 0.0f))}
	 
# # f64
# func $add_f64_u () f64 { 
   # return (
     # add f64(dread f64 $g_f64_u, constval f64 0))}

# func $add_f64 () f64 { 
   # return (
     # add f64(dread f64 $g_f64, constval f64 0))}
 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
