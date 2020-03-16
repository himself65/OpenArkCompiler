#This test case covers the following items:
            
#MapleIR Direct read/write 
#Direct acfgrecesses are associated with scalar variables and fixed fields inside 
#structures. Direct accesses can be mapped to pseudo-register if the variable 
#or field has no alias.  
#In MAPLE IR, dassign and dread are the opcodes for direct assignments and 
#direct references; 

#Syntax
#dread<prim-type><var-name><field-id>
#dassign<var-name><field-id>(<rhs-expression>)
#Type syntax
#type <id-name><derived-type>
func $strcpy ( var %dest <*i8>, var %src <*i8> ) <*i8> 

type $People < struct{
     @name <[10] i8>, 
     @age i8 }>

type $Info < struct{
     @number i32,
     @employee <$People> }>

var $p <$People>
var $temp<[6] i8>= [ 104, 117, 97, 119, 101, 105 ]

func $main ( )i32 {
     var %a i8
     dassign %a ( constval i8 120 )
     var %b i8
     dassign %b ( dread i32 %a )
     var %i i32
     dassign %i ( constval i32 5 )
     var %ii i32
     dassign %ii ( dread i32 %i )
     var %j f32
     dassign %j ( constval f32 12.9f )
     var %jj f32
     dassign %jj ( dread f32 %j )
     var %k f64
     dassign %k ( constval f64 20.5 )
     var %kk f64
     dassign %kk ( dread f64 %k )
     var %p < $People >
     var %pp < $People >
     dassign $p 2 ( constval i32 25)
     # dassign $p 1 ( constval i32 0 )
     #call <pu-name>(<opnd0>,...,<opndn>)
     call &strcpy ( addrof a64 %p, addrof a64 $temp)
     call &strcpy ( addrof a64 %pp, addrof a64 %p)
     var %info < $Info >
     var %info2 < $Info >
     dassign %info 2( dread agg %p )
     dassign %info2 2( dread agg %info 2)
     return ( constval i32 444 )
}

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
