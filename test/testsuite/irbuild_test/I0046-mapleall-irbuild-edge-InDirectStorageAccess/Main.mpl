#indirect access are associated with pionters and arrays
#iassign<type><field-id>(<addr-expr>, <rhs-expr>)
# type gives the high level type of <addr-expr> and must be a pointer type
# <addr-expr> is computed to return an address
# <rhs-expr> return a value
#iread<prim-type><type><field-id>(<addr-expr>)
# 
func $new ( var %n i32 )a64 
func $strcpy ( var %dest <*i8>, var %src <*i8> )<*i8> 
type $People < struct{
     @name < * i8 >,
     @number < * i32> }>
func $addition ( var %a i32, var %b i32 )i32 {
     return(
	add i32( dread i32 %a, dread i32 %b ))}
var $name < [6] i8 > = [ 104, 117, 97, 119, 101, 105 ]
var $temp<[6] i8> = [ 104, 117, 97, 119, 101, 105 ]
func $main ( ) i32 {
     var %cPtr < * i8 >
     dassign %cPtr ( constval a64 0)
     dassign %cPtr (
     	     addrof a64 $name)
     
     var %cPtr1 i8
     dassign %cPtr1(
     	     iread i32 < * i8 >( dread a64 %cPtr ))
     dassign %cPtr(
     	     add u64(
	     	 dread a64 %cPtr,
		 constval u64 1))
     var %cPtr2 <* i8>
     dassign %cPtr2(
     	     add u64(
	     	 dread a64 %cPtr,
		 constval u64 1))
     
     var %iPtr1 <* i32 >
     var %iPtr2 <* i32 >	     
     iassign < * i32>(
     	     dread a64 %iPtr1,
	     constval i32 10)
     iassign < * i32 >(
     	     dread a64 %iPtr2,
	     iread i32 < * i32> (
	     	   dread a64 %iPtr1))
     var %sPtr <* $People >
     #call $new(
     #	  iread <>)
     call &new (constval u32 128)
     dassign %sPtr(
         regread a64 %%retval
         )
     call &strcpy ( dread a64 %sPtr 0 ,
     	  	    addrof a64 $temp)               
     iassign <* i32>(
     	     dread a64 %sPtr 0,
	     constval i32 5)
     var %i i32
     var %j i32
     var %aPtr1 < [10] i32 >
     var %aPtr2	< [10][10] i32 >  
     var %aPtr3	< [10][10][10] i32 >
     dassign %i ( constval i32 0 )
     while(
       lt i32 i32 ( dread i32 %i, constval i32 10 )){
       	  iassign < *[10] i32 >(
	  	  array 1 a64 < *[10] i32 >(addrof a64 %aPtr1, 
		  		          dread i32 %i),
                  constval i32 10 )

          iassign< *[10] i32 >(
	  	  array 1 a64 < *[10] i32 >( addrof a64 %aPtr1,
		  	                   dread i32 %i),
		  dread i32 %i )

          iassign < *[10] i32 >(
	  	  array 1 a64 < *[10] i32 >( addrof a64 %aPtr1,
		  	      	    	   dread i32 %i ),
		  iread i32 <* i32 >(
		  	addrof a64 %aPtr1 ))
       dassign %i(
	 	 add i32(
		     dread i32 %i,	
		     constval i32 1))}


     dassign %i ( constval i32 0 )
     while(
	lt i32 i32 ( dread i32 %i, constval i32 10 )){
	dassign %j ( constval i32 0 )
	while(
		lt i32 i32 ( dread i32 %j, constval i32 10 )){
		   iassign < *[10][10] i32 >( 
		   	   array 1 a64 < *[10][10] i32 >( addrof a64 %aPtr2,
		   	   	 dread i32 %i,
				 dread i32 %j),
		           constval i32 10 )
	           iassign < *[10][10] i32 >(
		   	  array 1 a64< *[10][10] i32 >( addrof a64 %aPtr2,
			  	dread i32 %i,
				dread i32 %j),
			  mul i32 (
			      dread i32 %i,
			      dread i32 %j))
		   iassign< *[10][10] i32 >(
		   	    array 1 a64 < *[10][10] i32 >( addrof a64 %aPtr2,
			    	  dread i32 %i,
				  dread i32 %j),
		            iread i32 < *[10] i32 >(
			    	  array 1 a64< *[10] i32 >(
				  	addrof a64 %aPtr1,
				  	rem i32 (
					    mul i32 (
					    	dread i32 %i,
						dread i32 %j),
					    constval i32 10))))
                dassign %j(
			add i32(
		 	    dread i32 %j,	
		     	    constval i32 1))}
     dassign %i(
     	     add i32(
	     	 dread i32 %i,	
		 constval i32 1))}
     var %k i32
     dassign %i ( constval i32 0)
     while(
	lt i32 i32 ( dread i32 %i, constval i32 10 )){
	dassign %j ( constval i32 0)
	while(
	lt i32 i32 ( dread i32 %j, constval i32 10)){
	   while(
		lt i32 i32 ( dread i32 %k, constval i32 10)){
		iassign < *[10][10][10] i32 >(
			array 1 a64 < *[10][10][10] i32 >(addrof a64 %aPtr3,
			dread i32 %i, 
			dread i32 %j,
			dread i32 %k),
			constval i32 10)
		iassign < *[10][10][10] i32 >(
			array 1 a64 < *[10][10][10] i32>(addrof a64 %aPtr3,
			dread i32 %i, 
			dread i32 %j,
			dread i32 %k),
			mul i32 (
			    mul i32 (
			    	dread i32 %i,
				dread i32 %j),
			    dread i32 %k))
	        iassign < *[10][10][10] i32 >(
			array 1 a64 < *[10][10][10] i32 >(addrof a64 %aPtr3,
			dread i32 %i, 
			dread i32 %j,
			dread i32 %k),
			mul i32 (
			    iread i32 < *[10][10] i32>(
			    	  array 1 a64 <*[10][10] i32>( addrof a64 %aPtr1,
				  dread i32 %i,
				  dread i32 %j)),
				  constval i32 10))
 		dassign %k(
			add i32(
			    dread i32 %k,
			    constval i32 1))}
         dassign %j(
	 	 add i32(
		     dread i32 %j,	
		     constval i32 1))}
    dassign %i(
    	    add i32(
	    	dread i32 %i,	
		constval i32 1))}
    
    var %fPtr1 <func( i32, i32) i32 >
    var %fPtr2 <func( i32, i32) i32 >
    
    dassign %fPtr1(
          addrof a64 $addition)
    dassign %fPtr2(
          addrof a64 $addition)
    var %result i32
    icall ( dread a64 %fPtr1, constval i32 2, constval i32 3)
    dassign %result(
    	    regread i32 %%retval)
    return ( dread i32 %result)}
			
    

 # EXEC: %irbuild Main.mpl
 # EXEC: %irbuild Main.irb.mpl
 # EXEC: %cmp Main.irb.mpl Main.irb.irb.mpl
