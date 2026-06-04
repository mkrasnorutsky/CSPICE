#!/usr/bin/env bash
git commit -m "Release $1"
git tag "$1"
git push origin main
git push origin "$1"
