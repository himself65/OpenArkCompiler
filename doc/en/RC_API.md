RC API
----

Reference counting (RC) is a programming technique of storing the number of references to a resource, such as an object, a block of memory, disk space, and others, and releasing the resource when the number of references becomes 0. RC is used to achieve automatic resource management. RC also refers to a garbage collection algorithm that deallocates objects which are no longer referenced.

To support RC, OpenArkCompiler provides the following APIs for better code generation.

## void MCC\_IncRef\_NaiveRCFast(address\_t obj)

**Function:**

Increments RC of the object.

**Input parameter:**

obj: pointer of the heap object

**Return value:**

None

## void MCC\_DecRef\_NaiveRCFast(address\_t obj)

**Function:**

Decrements RC of the object.

**Input parameter:**

obj: pointer of the heap object

**Return value:**

None

## void MCC\_ClearLocalStackRef(address\_t \*addr)

**Function:**

Clears local reference on the thread stack and decrements RC for the stored reference.

**Input parameter:**

addr: address of the local reference on the thread stack

**Return value:**

None

## void MCC\_IncDecRef\_NaiveRCFast(address\_t incObj, address\_t decObj)

**Function:**

Increments RC for the object to which incObj points and decrements RC for the object to which decObj points.

**Input parameter:**

incObj: address of the object whose RC needs increment

incObj: address of the object whose RC needs decrement

**Return value:**

None

## void MCC\_IncDecRefReset(address\_t incObj, address\_t \*decAddr)

**Function:**

Increments RC for the object to which incObj points, decrements RC for the local variable object stored on the stack address pointer decAddr, and clears the memory to which the stack address pointer decAddr points.

**Input parameter:**

incObj: heap object whose RC needs increment

decAddr: address of the local reference on the stack

**Return value:**

None

## void MCC\_DecRefResetPair(address\_t \*decAddr0, address\_t \*decAddr1)

**Function:**

Clears the stack address space to which all parameters point, and decrements RC for the old value of the local variable.

**Input parameter:**

decAddr0 and decAddr1: addresses of the local reference on the stack

**Return value:**

None

## void MCC\_SetObjectPermanent(address\_t obj)

**Function:**

Sets a heap object to be permanently valid. After being invoked, RC for the object reaches the maximum value.

**Input parameter:**

obj: address of the heap object

**Return value:**

None

## address\_t MCC\_LoadVolatileStaticField(address\_t \*fieldAddr)

**Function:**

Obtains the value of the volatile static variable and increments RC for the fetched heap object.

**Input parameter:**

fieldAddr: address of the volatile static variable

**Return value:**

Returns the value of the volatile static variable.

## address\_t MCC\_LoadRefStatic(address\_t \*fieldAddr)

**Function:**

Obtains the value of the static variable and increments RC for the fetched heap object.

**Input parameter:**

fieldAddr: address of the static variable

**Return value**

Returns the value of the static variable.

## address\_t MCC\_LoadVolatileWeakField(address\_t obj, address\_t \*fieldAddr)

**Function:**

Obtains the value of the volatile variable marked by the weak annotation. If a non-null heap object is obtained, RC for the object will be incremented.

**Input parameter:**

obj: address of the heap object

fieldAddr: address of the volatile variable marked as weak

**Return value:**

Returns the value of the volatile variable marked as weak. A null object pointer may be returned.

## address\_t MCC\_LoadWeakField(address\_t obj, address\_t \*field\_addr)

**Function:**

Obtains the value of the variable marked by the weak annotation. If a non-null heap object is obtained, RC for the object will be incremented.

**Input parameter:**

obj: address of the heap object

fieldAddr: address of the variable marked as weak

**Return value:**

Returns the value of the variable marked as weak. A null object pointer may be returned.

## address\_t MCC\_LoadRefField\_NaiveRCFast(address\_t obj, address\_t \*fieldAddr)

**Function:**

Obtains the value of the fieldAddr variable, and increments RC for the obtained heap object.

**Input parameter:**

obj: address of the heap object

fieldAddr: address of the variable

**Return value:**

Returns the value of the variable.

## address\_t MCC\_LoadVolatileField(address\_t obj, address\_t \*fieldAddr)

**Function:**

Obtains the value of the volatile variable, and increments RC for the fetched heap object.

**Input parameter:**

obj: address of the heap object

fieldAddr: address of the volatile variable

**Return value:**

Returns the value of the volatile variable.

## void MCC\_WriteReferent(address\_t obj, address\_t value)

**Function:**

Store an object to the referent field of a java.lang.ref.Reference object. If a non-null heap object is obtained, RC for the object is incremented.

**Input parameter:**

obj: address of java.lang.ref.Reference

value: address of the heap object

**Return value:**

None

## void MCC\_WriteVolatileStaticFieldNoInc(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile static variable. This does not change RC for the heap object, but decrements RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the volatile static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileStaticFieldNoDec(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile static variable. This increments RC for the heap object, but does not decrement RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the volatile static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileStaticFieldNoRC(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile static variable. This does not change RC for the new value (value) or the old value (value of fieldAddr).

**Input parameter:**

fieldAddr: address of the volatile static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileStaticField(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile static variable. This increments RC for the heap object, and decrements RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the volatile static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldStaticNoInc(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the static variable. This does not increment RC for the heap object, but decrements RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldStaticNoDec(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the static variable. This increments RC for the heap object, but does not decrement RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldStaticNoRC(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the static variable. This does not increment RC for the heap object or decrement RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldStatic(address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the static variable. This increments RC for the heap object, and decrements RC for the old value of the static variable.

**Input parameter:**

fieldAddr: address of the static variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileFieldNoInc(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile variable. This does not increment RC for the heap object, but decrements RC for the old value of the volatile variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the volatile variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileFieldNoDec(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile variable. This increments RC for the heap object, but does not decrement RC for the old value of the volatile variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the volatile variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileFieldNoRC(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile variable. This does not increment RC for the heap object or decrement RC for the old value of the volatile variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the volatile variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileField(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile variable. This increments RC for the heap object, and decrements RC for the old value of the volatile variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the volatile variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldNoInc(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the variable. This does not increment RC for the heap object, but decrements RC for the old value of the variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldNoDec(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the variable. This increments RC for the heap object, but does not decrement RC for the old value of the variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefFieldNoRC(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the variable. This does not increment RC for the heap object or decrement RC for the old value of the variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteRefField(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the variable. This increments RC for the heap object, and decrements RC for the old value of the variable.

**Input parameter:**

obj: address of the object

fieldAddr: address of the variable

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteVolatileWeakField(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the volatile variable marked by Weak annotation. This increments RC for the heap object, and decrements RC for the old value of the volatile variable.

**Input parameter:**

obj: address of the object

fieldAddr: field of the volatile variable marked by the weak annotation

value: address of the heap object to be written

**Return value:**

None

## void MCC\_WriteWeakField(address\_t obj, address\_t \*fieldAddr, address\_t value)

**Function:**

Writes a heap object to the variable marked by the weak annotation. This increments RC for the heap object, and decrements RC for the old value of the variable.

**Input parameter:**

obj: address of the object

fieldAddr: field of the variable which marked by weak annotation

value: address of the heap object to be written

**Return value:**

None

