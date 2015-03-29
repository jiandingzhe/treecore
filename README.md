treejuce
========

a reforged JUCE core library with CMake build control and more regular project layout

Introduction
------------

We found JUCE library very useful, but we are not satisfied with JUCE's project layout, so we reformed it and added/modified some features.

  - everything in *treejuce* namespace.
  - SIMD operations.
  - atomic operations wrapped into functions, instead of objects.
  - picked out network system because we don't need it.
  - reforged reference counted object class and its holder class.
  - system config macros are generated by CMake proxy and file generation, not by a series of macro proxy.
  - use MT19937 to replace juce::Random.

