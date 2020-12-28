# Boost.SmartPtr: unshared_ptr<T>

## Motivation

There is a problem with `unique_ptr<T>`: it requires that `T` is complete. This precludes its use in many cases where we need pointers.

We have `shared_ptr<T>`, which solves that problem but it provides shared ownership and shared access to the object.

`unshared_ptr` is a `unique_ptr`-equivalent that is integrated with the rest of the Boost.SmartPtr API.

## Description

Like `unique_ptr<T>`, `unshared_ptr<T>` enforces at run-time the invariant that there are no more than one pointer instance that can access the object, but it does not require `T` to be complete. It is fully integrated in the Boost.SmartPtr family:

* Shares the same control block as `shared_ptr` and `weak_ptr` at no extra cost.

* Can be constructed from anything a `shared_ptr` can be constructed from.

* Can be constructed from a `shared_ptr` or from a `weak_ptr`:

	- The constructor throws if another `unshared_ptr` instance exists that shares the same control block.

* The free function template `unshare` constructs an `unshared_ptr` from a `shared_ptr` or from a `weak_ptr` without throwing exceptions.

* A `shared_ptr` can be constructed from a `unshared_ptr`:

	- The constructor throws if the `unshared_ptr` object is empty.

* A `weak_ptr` can be constructed from a `unshared_ptr`.

* The free function `share` constructs a `shared_ptr` from an `unshared_ptr` without throwing exceptions.

* Accessing a `shared_ptr` while an instance of `unshared_ptr` exists that shares the same control block is ill-formed (asserts).

Even though `unshared_ptr` restricts concurrent access to the object, it retains the shared ownership semantics of `shared_ptr` and `weak_ptr`. This means that the object will be destroyed when the last `shared_ptr` or `unshared_ptr` instance expires.
