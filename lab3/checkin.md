---
permalink: /labs/lab3/checkin/
title: 'Lab 3: Check-in'
---

1. How does the saved `pc` in a function's stack frame relate to the address of the first instruction for the function? If you take the saved `pc` in the stack frame for `main` and subtract it from the saved `lr` in the stack frame for `diff`, what do you get? Express your answer as a number, then explain conceptually what that computation represents. Here is the [stack diagram](../images/stack_abs.html) to use as a reference.

2. Why will loopback mode only work if you plug in both `TX` and `RX`?

3. On a hosted system, executing an incorrect call to `strlen` (e.g. argument is an invalid address or unterminated string) can result in a runtime error (crash/halt). But when running bare metal on the Pi, every call to `strlen` (whether well-formed or not) will complete "successfully" and return a value. Explain the difference in behavior. What is the return value for an erroneous call?

![](https://imgs.xkcd.com/comics/compiler_complaint.png)
