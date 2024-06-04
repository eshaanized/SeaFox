// |reftest| shell-option(--enable-arraybuffer-resizable) shell-option(--enable-float16array) skip-if(!ArrayBuffer.prototype.resize||!xulRuntime.shell) -- resizable-arraybuffer is not enabled unconditionally, requires shell-options
// Copyright (C) 2021 the V8 project authors. All rights reserved.
// This code is governed by the BSD license found in the LICENSE file.
/*---
esid: sec-%typedarray%.prototype.filter
description: Instance buffer can be resized during iteration
includes: [testTypedArray.js, compareArray.js]
features: [TypedArray, resizable-arraybuffer]
---*/

// If the host chooses to throw as allowed by the specification, the observed
// behavior will be identical to the case where `ArrayBuffer.prototype.resize`
// has not been implemented. The following assertion prevents this test from
// passing in runtimes which have not implemented the method.
assert.sameValue(typeof ArrayBuffer.prototype.resize, 'function');

testWithTypedArrayConstructors(function(TA) {
  var BPE = TA.BYTES_PER_ELEMENT;
  var NaNvalue = isFloatTypedArrayConstructor(TA) ? NaN : 0;
  var buffer = new ArrayBuffer(BPE * 3, {maxByteLength: BPE * 4});
  var sample = new TA(buffer);
  var finalElement, finalResult, expectedElements, expectedIndices, expectedArrays;
  var elements, indices, arrays, result;

  elements = [];
  indices = [];
  arrays = [];
  result = sample.filter(function(element, index, array) {
    if (elements.length === 0) {
      try {
        buffer.resize(2 * BPE);
        finalElement = undefined;
        finalResult = NaNvalue;
        expectedElements = [0, 0];
        expectedIndices = [0, 1];
        expectedArrays = [sample, sample];
      } catch (_) {
        finalElement = 0;
        finalResult = 0;
        expectedElements = [0, 0, 0];
        expectedIndices = [0, 1, 2];
        expectedArrays = [sample, sample, sample];
      }
    }

    elements.push(element);
    indices.push(index);
    arrays.push(array);
    return true;
  });

  assert.compareArray(elements, [0, 0, finalElement], 'elements (shrink)');
  assert.compareArray(indices, [0, 1, 2], 'indices (shrink)');
  assert.compareArray(arrays, [sample, sample, sample], 'arrays (shrink)');
  assert.compareArray(result, [0, 0, finalResult], 'result (shrink)');

  elements = [];
  indices = [];
  arrays = [];
  result = sample.filter(function(element, index, array) {
    if (elements.length === 0) {
      try {
        buffer.resize(4 * BPE);
      } catch (_) {}
    }

    elements.push(element);
    indices.push(index);
    arrays.push(array);
    return true;
  });

  assert.compareArray(elements, expectedElements, 'elements (grow)');
  assert.compareArray(indices, expectedIndices, 'indices (grow)');
  assert.compareArray(arrays, expectedArrays, 'arrays (grow)');
  assert.compareArray(result, expectedElements, 'result (grow)');
});

reportCompare(0, 0);