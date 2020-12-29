# Boost.SmartPtr: unshared_ptr<T>

## Motivation

There is a problem with `unique_ptr<T>`: it requires that `T` is complete. This precludes its use in many cases where we need pointers.

We have `shared_ptr<T>`, which solves that problem but it provides shared ownership and shared access to the object.

`unshared_ptr` is a `unique_ptr`-equivalent that is integrated with the rest of the Boost.SmartPtr API.

## Description

Like `unique_ptr<T>`, `unshared_ptr<T>` enforces at run-time the invariant that there are no more than one pointer instance that can access the object, but it does not require `T` to be complete. It is fully integrated in the Boost.SmartPtr family:

* Shares the same control block as `shared_ptr` and `weak_ptr` at no extra cost.

* Can be constructed from anything a `shared_ptr` can be constructed from.

* Can be constructed from a unique `shared_ptr` (the constructor throws otherwise)

	- `shared_ptr<T>::unshare()` is a no-throw alternative.

* A `shared_ptr` can be constructed from a `unshared_ptr`.

* While an object is managed by `unshared_ptr`, calling `weak_ptr<T>::lock()` results in an empty `shared_ptr`.