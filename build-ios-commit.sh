git commit -m "Release $1"
git tag $1
If you also want to push both the commit and the tag to the remote:
git push origin main
git push origin $1
