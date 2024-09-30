# Wats

Wats is designed to be a higher level WAT (WebAssembly Text Format) format. It is a transpiler which converts Wats program to Wat format.

# Example: 

Following is a Wats program:

```
export function add (i32 x, i32 y ) return i32 {
   return x + y
}

```

This program is transpiled to :

```wat
(func (export "add") (param $x i32) (param $y i32) (result i32)

   local.get $x
   local.get $y
   i32.add

)

```
