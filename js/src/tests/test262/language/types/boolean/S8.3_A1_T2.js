// Copyright 2009 the Sputnik authors.  All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
info: The Boolean type have two values, called true and false
es5id: 8.3_A1_T2
description: Check type of true/false and its equality
---*/

//////////////////////////////////////////////////////////////////////
// CHECK#1
if (typeof(true) !== "boolean") {
  throw new Test262Error('#1: typeof(true) === "boolean"');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#2
if (typeof(true) != "boolean") {
  throw new Test262Error('#2: typeof(true) == "boolean"');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#3
if (typeof(false) !== "boolean") {
  throw new Test262Error('#3: typeof(false) === "boolean"');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#4
if (typeof(false) != "boolean") {
  throw new Test262Error('#4: typeof(false) == "boolean"');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#5
if (true === false) {
  throw new Test262Error('#5: true !== false');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#6
if (true == false) {
  throw new Test262Error('#6: true != false');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#7
if (false === true) {
  throw new Test262Error('#7: false !== true');
}
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// CHECK#8
if (false == true) {
  throw new Test262Error('#8: false != true');
}
//
//////////////////////////////////////////////////////////////////////

reportCompare(0, 0);