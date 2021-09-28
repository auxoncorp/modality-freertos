#!/usr/bin/env bash

SUT="m3-qemu"
SESSION="qemu"

modality session delete $SESSION --force

modality sut delete $SUT --force

set -e

modality sut create .

modality sut use $SUT

modality session open $SESSION $SUT

modality session use $SESSION

exit 0
