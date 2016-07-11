Relax
=====

Why Relax ?
-----------
During last holiday I wanted to learn more about redux and also learn C++14,
so here it is.

Is this project used by anyone/completed/useful ?
-------------------------------------------------
No, No, Maybe.

Will you add more stuff from redux (eg: combiners) ?
---------------------------------------------------
Probably but they will be a bit different due to C++ and JS differences.


Is it thread safe ?
-------------------
**No**
the reducer works on copies, and if you apply relax correctly so that
State(n+1) = State(n) * Action(x)
Then you can think of it as sort of safe, but side effects may happen
when used in a threaded context and the code itself is not safe at all.

`pair<uint64_t, function<void(void)>> pair` should be `auto pair`
------------------------------------------------------------------
I figured out that leaving the first makes this more ugly,
but at the same time usable by C++11 projects as well as C++14.
