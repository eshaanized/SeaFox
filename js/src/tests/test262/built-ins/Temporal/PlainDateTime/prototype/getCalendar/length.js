// |reftest| skip-if(!this.hasOwnProperty('Temporal')) -- Temporal is not enabled unconditionally
// Copyright (C) 2023 Igalia, S.L. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.

/*---
esid: sec-temporal.plaindatetime.prototype.getcalendar
description: Temporal.PlainDateTime.prototype.getCalendar.length is 0
info: |
    Every built-in function object, including constructors, has a "length" property whose value is
    an integer. Unless otherwise specified, this value is equal to the largest number of named
    arguments shown in the subclause headings for the function description. Optional parameters
    (which are indicated with brackets: [ ]) or rest parameters (which are shown using the form
    «...name») are not included in the default argument count.

    Unless otherwise specified, the "length" property of a built-in function object has the
    attributes { [[Writable]]: false, [[Enumerable]]: false, [[Configurable]]: true }.
includes: [propertyHelper.js]
features: [Temporal]
---*/

verifyProperty(Temporal.PlainDateTime.prototype.getCalendar, "length", {
  value: 0,
  writable: false,
  enumerable: false,
  configurable: true,
});

reportCompare(0, 0);