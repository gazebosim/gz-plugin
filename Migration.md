# Note on deprecations
A tick-tock release cycle allows easy migration to new software versions.
Obsolete code is marked as deprecated for one major release.
Deprecated code produces compile-time warnings. These warning serve as
notification to users that their code should be upgraded. The next major
release will remove the deprecated code.

## Gazebo Plugin 1.X to 2.X

* The `ignition` namespace is deprecated and will be removed in future versions.
  Use `gz` instead.

* Header files under `ignition/...` are deprecated and will be removed in future versions.
  Use `gz/...` instead.
