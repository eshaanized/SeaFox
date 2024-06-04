/* Any copyright is dedicated to the Public Domain.
   http://creativecommons.org/publicdomain/zero/1.0/ */

"use strict";

const JS_CODE = `
window.onclick = function foo() {
  setTimeout(function bar() {
    dump("click and timed out\n");
  });
};
`;
const TEST_URL =
  "data:text/html,<!DOCTYPE html><html><script>" + JS_CODE + " </script>";

add_task(async function testTracingWorker() {
  const tab = await BrowserTestUtils.openNewForegroundTab(gBrowser, TEST_URL);

  await SpecialPowers.spawn(tab.linkedBrowser, [], async () => {
    const { JSTracer } = ChromeUtils.importESModule(
      "resource://devtools/server/tracer/tracer.sys.mjs",
      { global: "shared" }
    );

    // We have to fake opening DevTools otherwise DebuggerNotificationObserver wouldn't work
    // and the tracer wouldn't be able to trace the DOM events.
    ChromeUtils.notifyDevToolsOpened();

    const frames = [];
    const listener = {
      onTracingFrame(frameInfo) {
        frames.push(frameInfo);
      },
    };
    info("Register a tracing listener");
    JSTracer.addTracingListener(listener);

    info("Start tracing the iframe");
    JSTracer.startTracing({ global: content, traceDOMEvents: true });

    info("Dispatch a click event on the iframe");
    EventUtils.synthesizeMouseAtCenter(
      content.document.documentElement,
      {},
      content
    );

    info("Wait for the traces generated by this click");
    await ContentTaskUtils.waitForCondition(() => frames.length == 2);

    const firstFrame = frames[0];
    is(firstFrame.formatedDisplayName, "λ foo");
    is(firstFrame.currentDOMEvent, "DOM | click");

    const lastFrame = frames.at(-1);
    is(lastFrame.formatedDisplayName, "λ bar");
    is(lastFrame.currentDOMEvent, "setTimeoutCallback");

    JSTracer.stopTracing();
    JSTracer.removeTracingListener(listener);

    ChromeUtils.notifyDevToolsClosed();
  });

  BrowserTestUtils.removeTab(tab);
});